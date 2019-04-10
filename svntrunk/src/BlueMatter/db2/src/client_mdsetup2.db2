catalog tcpip node pepsi
 remote pepsi.watson.ibm.com
 server 60017
 with 'hadr primary for mdsetup2';
catalog database mdsetup2 as mdsetup
 at node pepsi
 authentication CLIENT;
