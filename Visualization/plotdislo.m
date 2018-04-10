function plotdislo(x,y,dislo,segtype)
for i=1:length(segtype)
    if segtype(i)==0
        list = [dislo(i) dislo(i+1)];
        plot(x(list),y(list),'-r')
        hold on
    end
    if segtype(i)==1
        list = [dislo(i) dislo(i+1)];
        plot([x(list(1));x(list(2))+1],y(list),'-y')
        plot([x(list(1))-1;x(list(2))],y(list),'-y')
        hold on
    end
    if segtype(i)==2
        list = [dislo(i) dislo(i+1)];
        plot([x(list(1));x(list(2))-1],y(list),'-k')
        plot([x(list(1))+1;x(list(2))],y(list),'-k')
        hold on
    end
end
end