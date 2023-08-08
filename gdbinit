# Craig Scratchley, 2023
# gdb initialization file for ENSC ?51

set debuginfod enabled on
 
#set debug infrun 1
set logging overwrite on
set logging on

skip -gfi /usr/include/c++/*/*/*
skip -gfi /usr/include/c++/*/*
skip -gfi /usr/include/c++/*

skip -gfi /usr/include/fmt/*

#set history expansion
