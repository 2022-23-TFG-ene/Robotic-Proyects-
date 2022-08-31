
teta1=0 ,d1=10 ,a1=0
alfa1=10
teta2=0
d2=0
a2=0
alfa2=10
teta3=40
d3=0
a3=0
alfa3=10
teta4=0
d4=0
a4=0
alfa4=10
teta5=0
d5=10
a5=0
alfa5=10
teta6=40
d6=0
a6=0
alfa6=10
teta7=0
d7=0
a7=0
alfa7=10

A=[cosd(teta1) -cosd(alfa1)*sind(teta1) sind(alfa1)*sind(teta1) a1*cosd(teta1);
   sind(teta1) cosd(alfa1)*cosd(teta1) -sind(alfa1)*cosd(teta1) a1*sind(teta1);
   0 sind(alfa1) cosd(alfa1) d1;
   0 0 0 1]

B=[cosd(teta2) -cosd(alfa2)*sind(teta2) sind(alfa2)*sind(teta2) a2*cosd(teta2);
   sind(teta2) cosd(alfa2)*cosd(teta2) -sind(alfa2)*cosd(teta2) a2*sind(teta2);
   0 sind(alfa2) cosd(alfa2) d2;
   0 0 0 1]

C=[cosd(teta3) -cosd(alfa3)*sind(teta3) sind(alfa3)*sind(teta3) a3*cosd(teta3);
   sind(teta3) cosd(alfa3)*cosd(teta3) -sind(alfa3)*cosd(teta3) a3*sind(teta3);
   0 sind(alfa3) cosd(alfa3) d3;
   0 0 0 1]

D=[cosd(teta4) -cosd(alfa4)*sind(teta4) sind(alfa4)*sind(teta4) a4*cosd(teta4);
   sind(teta4) cosd(alfa4)*cosd(teta4) -sind(alfa4)*cosd(teta4) a4*sind(teta4);
   0 sind(alfa4) cosd(alfa4) d4;
   0 0 0 1]

E=[cosd(teta5) -cosd(alfa5)*sind(teta5) sind(alfa5)*sind(teta5) a5*cosd(teta5);
   sind(teta5) cosd(alfa5)*cosd(teta5) -sind(alfa5)*cosd(teta5) a5*sind(teta5);
   0 sind(alfa5) cosd(alfa5) d5;
   0 0 0 1]

F=[cosd(teta6) -cosd(alfa6)*sind(teta6) sind(alfa6)*sind(teta6) a6*cosd(teta6);
   sind(teta6) cosd(alfa6)*cosd(teta6) -sind(alfa6)*cosd(teta6) a6*sind(teta6);
   0 sind(alfa6) cosd(alfa6) d6;
   0 0 0 1]

G=[cosd(teta7) -cosd(alfa7)*sind(teta7) sind(alfa7)*sind(teta7) a7*cosd(teta7);
   sind(teta7) cosd(alfa7)*cosd(teta7) -sind(alfa7)*cosd(teta7) a7*sind(teta7);
   0 sind(alfa7) cosd(alfa7) d7;
   0 0 0 1]


H=A*B*C*D*E*F*G

















