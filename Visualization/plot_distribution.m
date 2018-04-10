
fid = fopen('test.xy0');

i=0;
while 1
    i = i+1;
    tline = fgetl(fid);
    if ischar(tline)
        xytheta = str2num(tline);
        x(i)=xytheta(1,1);
        y(i)=xytheta(1,2);
        theta(i)=xytheta(1,3);
    else
        break;
    end
end

plot(x,y,'.');
axis square
