//============================================================================
// gf_topological_dag.v
// Synthesizable Digital Verilog RTL + Testbench:
// Finite-Field GF(2^4) Arithmetic over a Sparse Ramanujan-Expander Topological
// DAG with Parity-Check Error Detection & Holographic Boundary Entropy
//============================================================================

`timescale 1ns/1ps

module gf16_mul (
    input  [3:0] a,
    input  [3:0] b,
    output [3:0] y
);
    // Carry-less polynomial product a(x) * b(x) in GF(2)[x]
    wire [6:0] p;
    assign p[0] = (a[0] & b[0]);
    assign p[1] = (a[0] & b[1]) ^ (a[1] & b[0]);
    assign p[2] = (a[0] & b[2]) ^ (a[1] & b[1]) ^ (a[2] & b[0]);
    assign p[3] = (a[0] & b[3]) ^ (a[1] & b[2]) ^ (a[2] & b[1]) ^ (a[3] & b[0]);
    assign p[4] = (a[1] & b[3]) ^ (a[2] & b[2]) ^ (a[3] & b[1]);
    assign p[5] = (a[2] & b[3]) ^ (a[3] & b[2]);
    assign p[6] = (a[3] & b[3]);

    // Reduction modulo P(x) = x^4 + x + 1:
    //   x^4 = x + 1, x^5 = x^2 + x, x^6 = x^3 + x^2
    assign y[0] = p[0] ^ p[4];
    assign y[1] = p[1] ^ p[4] ^ p[5];
    assign y[2] = p[2] ^ p[5] ^ p[6];
    assign y[3] = p[3] ^ p[6];
endmodule

module gf16_dag_node (
    input  [3:0] in0,
    input  [3:0] in1,
    input  [3:0] in2,
    input  [3:0] coeff,
    output [3:0] out
);
    wire [3:0] sum_gf;
    assign sum_gf = in0 ^ in1 ^ in2; // GF(2^4) addition across d=3 Ramanujan neighbors

    gf16_mul u_mul (
        .a(sum_gf),
        .b(coeff),
        .y(out)
    );
endmodule

module gf_topological_dag #(
    parameter N = 8
)(
    input  wire        clk,
    input  wire        rst_n,
    input  wire [31:0] bulk_flat,         // 8 x 4-bit GF(2^4) bulk nodes
    input  wire [3:0]  coeff_l1,          // Layer 1 GF(2^4) scaling
    input  wire [3:0]  coeff_l2,          // Layer 2 GF(2^4) scaling
    input  wire [31:0] bulk_fault_mask,   // Optional fault/erasure injection at Layer 1
    output reg  [31:0] bnd_flat,          // 8 x 4-bit GF(2^4) boundary nodes
    output reg  [3:0]  syndrome_gf,       // Holographic GF(2^4) parity syndrome (0 = valid)
    output reg  [5:0]  boundary_entropy   // Active boundary bits (area-law proxy, 0..32)
);
    wire [3:0] bulk [0:N-1];
    wire [3:0] mid  [0:N-1];
    wire [3:0] mid_faulty [0:N-1];
    wire [3:0] bnd  [0:N-1];

    genvar i;
    generate
        for (i = 0; i < N; i = i + 1) begin : gen_unpack
            assign bulk[i] = bulk_flat[4*i +: 4];
        end

        // Layer 0 (Bulk) -> Layer 1 (Interior): 3-regular Ramanujan circulant C_8(1, 4)
        for (i = 0; i < N; i = i + 1) begin : gen_layer1
            localparam integer PREV = (i + N - 1) % N;
            localparam integer NEXT = (i + 1) % N;
            localparam integer OPP  = (i + 4) % N;

            gf16_dag_node u_l1 (
                .in0(bulk[PREV]),
                .in1(bulk[NEXT]),
                .in2(bulk[OPP]),
                .coeff(coeff_l1),
                .out(mid[i])
            );
            assign mid_faulty[i] = mid[i] ^ bulk_fault_mask[4*i +: 4];
        end

        // Layer 1 (Interior) -> Layer 2 (Boundary): 3-regular Ramanujan circulant C_8(1, 4)
        for (i = 0; i < N; i = i + 1) begin : gen_layer2
            localparam integer PREV = (i + N - 1) % N;
            localparam integer NEXT = (i + 1) % N;
            localparam integer OPP  = (i + 4) % N;

            gf16_dag_node u_l2 (
                .in0(mid_faulty[PREV]),
                .in1(mid_faulty[NEXT]),
                .in2(mid_faulty[OPP]),
                .coeff(coeff_l2),
                .out(bnd[i])
            );
        end
    endgenerate

    // Expected global holographic parity: c1 * c2 * sum(bulk[0..7])
    wire [3:0] bulk_parity;
    wire [3:0] bnd_parity;
    wire [3:0] c12, expected_bnd_parity;

    assign bulk_parity = bulk[0] ^ bulk[1] ^ bulk[2] ^ bulk[3] ^
                         bulk[4] ^ bulk[5] ^ bulk[6] ^ bulk[7];
    assign bnd_parity  = bnd[0]  ^ bnd[1]  ^ bnd[2]  ^ bnd[3]  ^
                         bnd[4]  ^ bnd[5]  ^ bnd[6]  ^ bnd[7];

    gf16_mul u_c12 (.a(coeff_l1), .b(coeff_l2), .y(c12));
    gf16_mul u_exp (.a(bulk_parity), .b(c12), .y(expected_bnd_parity));

    integer k, b;
    reg [5:0] popcount;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            bnd_flat         <= 32'd0;
            syndrome_gf      <= 4'd0;
            boundary_entropy <= 6'd0;
        end else begin
            popcount = 6'd0;
            for (k = 0; k < N; k = k + 1) begin
                bnd_flat[4*k +: 4] <= bnd[k];
                for (b = 0; b < 4; b = b + 1) begin
                    popcount = popcount + bnd[k][b];
                end
            end
            syndrome_gf      <= bnd_parity ^ expected_bnd_parity;
            boundary_entropy <= popcount;
        end
    end
endmodule

// -----------------------------------------------------------------------------
// Self-checking testbench
// -----------------------------------------------------------------------------
module tb_gf_topological_dag;
    reg         clk = 0;
    reg         rst_n = 0;
    reg  [31:0] bulk_flat;
    reg  [3:0]  coeff_l1, coeff_l2;
    reg  [31:0] bulk_fault_mask;
    wire [31:0] bnd_flat;
    wire [3:0]  syndrome_gf;
    wire [5:0]  boundary_entropy;

    always #5 clk = ~clk;

    gf_topological_dag dut (
        .clk(clk),
        .rst_n(rst_n),
        .bulk_flat(bulk_flat),
        .coeff_l1(coeff_l1),
        .coeff_l2(coeff_l2),
        .bulk_fault_mask(bulk_fault_mask),
        .bnd_flat(bnd_flat),
        .syndrome_gf(syndrome_gf),
        .boundary_entropy(boundary_entropy)
    );

    initial begin
        bulk_flat       = 32'hA53C_71E2;
        coeff_l1        = 4'h3; // x + 1 in GF(2^4)
        coeff_l2        = 4'h5; // x^2 + 1 in GF(2^4)
        bulk_fault_mask = 32'h0000_0000;

        #12 rst_n = 1;
        #10;
        $display("No-fault: boundary=%h syndrome=%h entropy=%0d",
                 bnd_flat, syndrome_gf, boundary_entropy);
        if (syndrome_gf !== 4'h0) $fatal(1, "Expected zero syndrome without fault");

        // Inject a 1-node interior fault in GF(2^4)
        bulk_fault_mask = 32'h0000_0009;
        #10;
        $display("Faulty:   boundary=%h syndrome=%h entropy=%0d",
                 bnd_flat, syndrome_gf, boundary_entropy);
        if (syndrome_gf === 4'h0) $fatal(1, "Expected non-zero syndrome on bulk fault");

        $display("PASS: All GF(2^4) topological DAG checks passed.");
        $finish;
    end
endmodule
