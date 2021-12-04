//
// Created by Shijie on 2021/12/3.
// Test the help function of project.c: decoder
//

#include <stdio.h>

typedef char BIT;
#define TRUE 1
#define FALSE 0

/******************************************************************************/
// help function for testing
/******************************************************************************/

BIT  not_gate(BIT A) {
    if (A)
        return FALSE;
    else
        return TRUE;
}

BIT  or_gate(BIT A, BIT B) {
    if (A || B)
        return TRUE;
    else
        return FALSE;
}

BIT  or_gate3(BIT A, BIT B, BIT C) {
    return or_gate(A, or_gate(B, C));
}

BIT  and_gate(BIT A, BIT B) {
    if (A && B)
        return TRUE;
    else
        return FALSE;
}

BIT  and_gate3(BIT A, BIT B, BIT C) {
    return and_gate(A, and_gate(B, C));
}

/******************************************************************************/
// function need test
/******************************************************************************/

void decoder2(BIT I0, BIT I1, BIT* O0, BIT* O1, BIT* O2, BIT* O3) {
    // Note: The input -> output mapping is slightly modified from before
    *O0 = and_gate(not_gate(I1), not_gate(I0));
    *O1 = and_gate(not_gate(I1), I0);
    *O2 = and_gate(I1, not_gate(I0));
    *O3 = and_gate(I1, I0);
}

void decoder3(BIT* I, BIT EN, BIT* O) {
    // TODO: implement 3-to-8 decoder using gates
    O[0] = and_gate3(not_gate(I[2]), not_gate(I[1]), not_gate(I[0]));
    O[1] = and_gate3(not_gate(I[2]), not_gate(I[1]), I[0]);
    O[2] = and_gate3(not_gate(I[2]), I[1], not_gate(I[0]));
    O[3] = and_gate3(not_gate(I[2]), I[1], I[0]);
    O[4] = and_gate3(I[2], not_gate(I[1]), not_gate(I[0]));
    O[5] = and_gate3(I[2], not_gate(I[1]), I[0]);
    O[6] = and_gate3(I[2], I[1], not_gate(I[0]));
    O[7] = and_gate3(I[2], I[1], I[0]);

    O[0] = and_gate(EN, O[0]);
    O[1] = and_gate(EN, O[1]);
    O[2] = and_gate(EN, O[2]);
    O[3] = and_gate(EN, O[3]);
    O[4] = and_gate(EN, O[4]);
    O[5] = and_gate(EN, O[5]);
    O[6] = and_gate(EN, O[6]);
    O[7] = and_gate(EN, O[7]);
}

void decoder5(BIT* I, BIT* O) {
    // TODO: implement 5-to-32 decoder using 2-to-4 and 3-to-8 decoders
    BIT EN[4] = {FALSE};
    decoder2(I[3], I[4], &EN[0], &EN[1], &EN[2], &EN[3]);
    decoder3(I, EN[3], &O[24]);
    decoder3(I, EN[2], &O[16]);
    decoder3(I, EN[1], &O[8]);
    decoder3(I, EN[0], &O[0]);
}


int main() {
    // Unit test for 5-to-32 decoder
    printf("\n===== Unit test for 5-to-32 decoder =====\n");
    printf("decoder( I4, I3, I2, I1, I0 ) | ( O31, O30, ..., O1, O0 )\n");
    printf("------------------------------|--------------------------------------------------------------------------------------------------\n");
    BIT I[5] = {0};
    BIT O[32] = {0};
    for (I[4] = 0; I[4] < 2; ++I[4])
        for (I[3] = 0; I[3] < 2; ++I[3])
            for (I[2] = 0; I[2] < 2; ++I[2])
                for (I[1] = 0; I[1] < 2; ++I[1])
                    for (I[0] = 0; I[0] < 2; ++I[0]) {
                        decoder5(I, O);
                        printf("decoder( %2d, %2d, %2d, %2d, %2d, ) | ( ",
                               I[4], I[3], I[2], I[1], I[0]);
                        printf("%2d", O[31]);
                        for (int i = 30; i >= 0; --i)
                            printf(", %2d", O[i]);
                        printf(" )\n");
                    }

    return 0;
}
