#pragma once

#define RST  "\x1B[0m"
#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLU  "\x1B[34m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define WHT  "\x1B[37m"

#define CRED(x) RED x RST
#define CGRN(x) GRN x RST
#define CYEL(x) YEL x RST
#define CBLU(x) BLU x RST
#define CMAG(x) MAG x RST
#define CCYN(x) CYN x RST
#define CWHT(x) WHT x RST

#define BOLD(x) "\x1B[1m" x RST
#define UNDL(x) "\x1B[4m" x RST