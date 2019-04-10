connect to mdsetup2;
create regular tablespace mdshort
 managed by automatic storage initialsize 10 G increasesize 256 M;
create long tablespace mdlong
 managed by automatic storage initialsize 64 M increasesize 64 M;
connect reset;
terminate;
