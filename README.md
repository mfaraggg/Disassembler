### Disassembler
Assembly project

##Implementation

 We started by the skeleton code provided to us by our professor.Then, we decided to divide the instruction types into separate functions, and each function outputs the specific instruction according to the BIN file. There is a function, named instDecExec, which checks the type of instruction by checking the opcode, and accordingly, calls the function of the corresponding instruction type. Also, there is a function for the compressed instructions, named compressedInst. While reading the BIN file, the program checks whether the first two bits are zeros or ones, to check if it is I-instructions or C-instructions. If it starts with (..11) then it is an I-instruction and the program will go to instDecExec to check the instruction type. Else, the programe then will go to compressedInst to check the instruction type.

##Limitations

  We cannot print the label names that are located in the original .s file, which makes it hard to place label names in the positions that they are called in and located.

##Known issues


##Contributions of each team member

  Nour: I-Type, J-Type, comments in code, worked on test cases
  Nagham: S-Type, concatenation of immediate values, made sure that program ends at the end of the bin file and not have a specific size, comments in code
  Mohammed: R-Type, B-Type, U-Type, added the ABI register names and implemented it in the code, comments in code

  We split up all the work between all the members, and we met up numerous times to work on the project together. For each part of the code, each person had some level of input, but the list above shows what each person mainly worked on. Then, we all worked together on the I-Type, U-Type and S-Type functions to make sure that the immediate values were printed out correctly, depending on the sign of the value. Overall, we all looked over each others codes to make sure there were no mistakes and made minor tweaks when required.
