AUTOMAKE_OPTIONS=foreign
bin_PROGRAMS=bview
bview_SOURCES=main.c eggIndex.c
bview_LDADD=-lglib-2.0 -legg3 
bview_CPPFLAGS=`pkg-config --cflags glib-2.0 egg3`  -L/usr/local/lib -L/usr/local/lib/ 
ACLOCAL_AMFLAGS =-I m4
