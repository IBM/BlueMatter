#! /bin/ksh

java java_cup.Main -parser Parser -symbols Constant < probspecdb2.cup

mv Parser.java parser/
mv Constant.java parser/
