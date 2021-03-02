File configurations/layouts
===========================

Test files for different configurations of TapeMarks and/or padding. All files
contain the same LIS data, which should be loaded correctly by `dlisio.lis.load`::

    *.lis
    |
    |-> Reel Header
        |
        |-> Tape Header
            |
            |-> File Header
            |-> File Trailer
            |-> File Header
            |-> File Trailer

That is, a single REEL, containing a singel TAPE, containg 2 Logical Files,
each containing only a FHLR and a FTLR. All trailer record are present. 

Note that the actual Logical Records contain dummy data and can't be parsed by
any parsing routine. 

Files named `broken_**.lis` do not adhere to specifications, and may or may not
be successfully loaded by dlisio.

=============== ==============================================================
Filename        Desciption
=============== ==============================================================
layout_01.lis   1 TM(0) before first PR
layout_02.lis   2 TM(0) before first PR
layout_03.lis   1 TM(1) and 1 TM(0) before first PR
layout_04.lis   1 TM(0) between LF's
layout_05.lis   2 TM(0) between LF's  
layout_06.lis   1 TM(1) and 1 TM(0) between LF's
layout_07.lis   1 TM(1) at EOF
layout_08.lis   2 TM(1) at EOF
layout_09.lis   3 TM(1) at EOF
layout_10.lis   1 TM(0) between LF's - Padding between PR's
layout_11.lis   2 TM(0) between LF's - Padding between PR's
layout_12.lis   1 TM(1) and 1 TM(0) between LF's - Padding between PR's

broken_01.lis   No TM at EOF

=============== ==============================================================
