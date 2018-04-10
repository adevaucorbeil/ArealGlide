function [max_CRSS,num_pinned,area]=post_analysis(filename)

fid = fopen(filename);
i=0;
while 1
    i = i+1;
    tline = fgetl(fid);
    if ischar(tline)
        line = str2num(tline);
        step(i)=line(1,1);
        CRSS(i)=line(1,2);
        num_pinned(i)=line(1,3);
        area(i)=line(1,4);
    else
        break
    end
end

for i=1:length(CRSS)
    max_CRSS(i)=max(CRSS(1:i));
end

end