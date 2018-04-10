function makeplot(segment,x,y,r)

    xn1 = x(segment(2));
    yn1 = y(segment(2));
    xn2 = x(segment(1));
    yn2 = y(segment(1));
    
    type = segment(3);
    
    if type == 0
        drawarc(xn1,xn2,yn1,yn2,r);
        drawarc(xn1+1,xn2+1,yn1,yn2,r);
        drawarc(xn1-1,xn2-1,yn1,yn2,r);
    end
    if (type == 1)
        drawarc(xn1+1,xn2,yn1,yn2,r);
        drawarc(xn1,xn2-1,yn1,yn2,r);
    end    
    if (type == 2)
        drawarc(xn1,xn2+1,yn1,yn2,r);
        drawarc(xn1-1,xn2,yn1,yn2,r);
    end    
end
