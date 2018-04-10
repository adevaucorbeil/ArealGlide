function [cn,xi,yi]=drawarc(xn1,xn2,yn1,yn2,r)

%If (x1,y1) and (x2,y2) are the beginning and end points, respectively, and r the
%radius of the desired arc which is to bend in a counterclockwise
%direction -- this draws the arc

d = sqrt((xn2-xn1)^2+(yn2-yn1)^2); % Distance between points
a = atan2(-(xn2-xn1),yn2-yn1); % Perpendicular bisector angle
b = asin(min(1,(d/2)/r)); % Half arc angle
c = linspace(a-b,a+b); % Arc angle range
e = sqrt(max(0,r^2-d^2/4)); % Distance, center to midpoint
xi = (xn1+xn2)/2-e*cos(a)+r*cos(c); % Cartesian coords. of arc
yi = (yn1+yn2)/2-e*sin(a)+r*sin(c);
plot(xi,yi,'k-','LineWidth',1.5); %,x1,y1,'ro',x2,y2,'bo')
hold on

cn = 0;
%[cn]=circcentre(x1,x2,y1,y2,r);

%  If you want the bending to go clockwise, just change the second line to:
%a = atan2(x2-x1,-(y2-y1));
