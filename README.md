# GaussJordanTests
Algorithm Applying concurrent programming to gauss jordan elimination

## Usage

`./main gaussJordan [multiSwitch] [n]`
`n` is the number of rows in the matrix. The number of columns will be `n+1` (like a system of equations!).  The multiSwitch is used to determine if there is one 
process working on the matrix or `n-1` processes working on it.
