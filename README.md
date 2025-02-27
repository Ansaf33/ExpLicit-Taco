### Objective
Develop a 3-address code generator for the Experimental Language (ExpL)
whose specification is given here (http://silcnitc.github.io/expl.html) and perform a code optimization known as Local
Common Subexpression Elimination in the generated code. 
For simplicity, you may omit composite data types, type definitions, user defined functions and dynamic memory
allocation from the ExpL specification. 

**Refer ALSU 6.2.1 for more details on 3-address code
format and ALSU 8.5 for details on local code optimization.**

Students must work in teams of 2. Grading will be based on your submission, tests involving
implementing small extensions to the work done and viva-voce.

## THREE ADDRESS CODE GENERATION
Three-Address Code (TAC) generation is an intermediate representation (IR) used in compilers that breaks down complex expressions into simple instructions, each involving at most three operands. This makes it easier to optimize and translate into machine code.

### Key Features of Three-Address Code:
- At Most Three Operands – Each instruction has one operator and at most three operands.
- Temporary Variables – Complex expressions are broken down using temporary variables.
- Linear Execution – TAC is often presented as a sequence of statements, simplifying control flow.

## LOCAL COMMON SUBEXPRESSION ELIMINATION

Local Common Subexpression Elimination (CSE) is an optimization technique that removes redundant computations within a basic block. A basic block is a sequence of instructions with no branches except at the beginning or the end.

### Concept
If an expression is computed more than once within a basic block and its operands haven’t changed, we can reuse the first computation instead of recalculating it.
