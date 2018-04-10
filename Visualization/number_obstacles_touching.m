function [CRSS,num_touching]=number_obstacles_touching(filename)

fid2 = fopen([filename,'.out']);
i=0;
while 1
    i = i+1;
    tline = fgetl(fid2);
    
    if ischar(tline)
        if isempty(find(tline=='B',1))
            CRSS_temp = str2num(tline(find(tline=='S',1)+2:find(tline=='T',1)-1));
        else
            CRSS_temp = str2num(tline(find(tline=='S',1)+2:find(tline=='B',1)-1));
        end
    
        if exist('CRSS','var')
            CRSS(i) = max([CRSS CRSS_temp]);
        else
            CRSS(i) = CRSS_temp;
        end
        temp1 = str2num(tline(1:find(tline=='C',1)-1)); %#ok<*ST2NM>
            
        dislo = temp1(2:end)+1;
        num_touching(i)=length(dislo)-1; %#ok<*AGROW>
    else
        break
    end
end

fprintf(1,['The number of obstacles pinning the dislocation at the pick stress is ',num2str(num_touching(find(CRSS==CRSS(end),1))),'\n']);

end