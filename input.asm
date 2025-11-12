    .text
start:
    addi    x1,  x0,  5     
    addi    x2,  x0, 10    


    add     x3,  x1,  x2       
    addw    x4,  x1,  x2     
    sub     x5,  x2,  x1       
    subw    x6,  x2,  x1     
    and     x7,  x1,  x2       
    or      x8,  x1,  x2       
    xor     x9,  x1,  x2      
    sll     x10, x2,  x1       
    srl     x11, x2,  x1       
    sra     x12, x2,  x1     
    slt     x13, x1,  x2       
    mul     x14, x2,  x1       
    mulw    x15, x2,  x1
    div     x16, x2,  x1
    divw    x17, x2,  x1
    rem     x18, x2,  x1
    remw    x19, x2,  x1

    addi    x20, x0, -3        
    addiw   x21, x0, 7        
    andi    x22, x21, 1        
    ori     x23, x21, 0xff     

    
    sb      x25, 0(x29)   
    sh      x26, 2(x29)   
    sw      x27, 4(x29)   
    sd      x28, 8(x29)   

    
    addi    x5, x0, 1
    addi    x6, x0, 1
    beq     x5, x6, BR_EQ
    
    sb      x0, 16(x30)
    jal     x0, BR_CONT
BR_EQ:
    addi    x31, x0, 1
    sb      x31, 16(x30)
BR_CONT:

    
    addi    x5, x0, 2
    addi    x6, x0, 3
    bne     x5, x6, BR_BNE_TAKEN
    
    sb      x0, 17(x29)
    jal     x0, BR_BNE_CONT
BR_BNE_TAKEN:
    sb      x31, 17(x29)
BR_BNE_CONT:

    
    addi    x5, x0, -1
    addi    x6, x0, 0
    blt     x5, x6, BR_BLT_TAKEN
    sb      x0, 18(x29)
    jal     x0, BR_BLT_CONT
BR_BLT_TAKEN:
    sb      x31, 18(x29)
BR_BLT_CONT:
    addi    x5, x0, 4
    addi    x6, x0, 4
    bge     x5, x6, BR_BGE_TAKEN
    sb      x0, 19(x29)
    jal     x0, BR_BGE_CONT
BR_BGE_TAKEN:
    sb      x31, 19(x29)
BR_BGE_CONT:

    
    
    jal     x1, JAL_TARGET     

    addi    x2, x2, 1

JAL_TARGET:
       addi    x31, x0, 0x7f
    sb      x31, 20(x30)

    addi    x5, x1, 0
    jalr    x0, x5 0

    
    


    lui     x6, 0x12345      
    auipc   x7, 0x0           

    sb      x31, 24(x29)


    addi    x10, x0, 0        
    addi    x17, x0, 93       



    .data
data_bytes: .byte 0x11, 0x22, 0x33, 0x44
data_half: .half 0x1234, 0x5678
data_word: .word 0x1234567
 .asciiz "hello-riscv64"

scratch:  .dword 0x0, 0x0, 0x0, 0x0 
  
