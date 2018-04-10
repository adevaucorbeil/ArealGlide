function plot_dislocation(output,input,plot_step)

% Get the obstacle list:

fobstacles = fopen(input,'r');
xy = fscanf(fobstacles, '%g %g %g', [3 inf])';
x = xy(:,1);    
y = xy(:,2);
clear xy;
x(1)=0.2;
x(2)=0.5;
x(3)=0.75;

y(1)=0;
y(2)=0;
y(3)=0;

alive = ones(size(x));

d = 1/sqrt(length(x));

fclose(fobstacles);

foutput = fopen(output,'r');
hf=figure('Position',[10 10 1000 500]);
hold on;

i=0;
while 1
    i=i+1;
    tline = fgetl(foutput);
    if ~ischar(tline), break, end
    
    c_loc = find(tline == 'C');
    type_loc = find(tline == 'T');
    
    temp = str2num(tline(1:c_loc-1));
    step = temp(1);
    clear dislocation;
    dislocation = temp(2:end);
    dislocation = dislocation +1;
    type = zeros(length(dislocation)-1);
    
    diff_type = str2num(tline(type_loc+5:end));
    
    for j=1:length(diff_type)/2
        type(diff_type(j*2-1)+1) = diff_type(j*2);
    end
    
    if (isempty(find(tline == 'B',1)))
        stress(i) = str2num(tline(c_loc+5:find(tline == 'T',1)-1));
    else
        stress(i) = str2num(tline(c_loc+5:find(tline == 'B',1)-1));
        broken_node = str2num(tline(find(tline == 'B',1) + 5:find(tline == 'T',1) -1)) + 1;
        alive(broken_node) = 0;
    end
    
    crss(i) = max(stress);
    
    if (step/plot_step - floor(step/plot_step)==0)
    if exist('h2','var')
        cla(h2)
    end
    h2 = subplot(1,2,2);
    plot(1:i,stress);
    hold on
    plot(1:i,crss,'r');
    xlabel('Step');
    ylabel('Required shear stress');
    
    h1 = subplot(1,2,1);
    hold on
    
    if exist('h1','var')
        cla(h1)
    end
    
    plot(x.*alive,y.*alive,'.b');
    plot((x+ones(size(x))).*alive,y.*alive,'.b');
    %plot((x-ones(size(x))).*alive,y.*alive,'.b');
    
    for k=1:length(dislocation)-1
        segment = [ dislocation(k), dislocation(k+1),type(k) ];
        makeplot(segment,x,y,d/(2*stress(i)));
    end
    
    axis([0 1 0 1]);
    drawnow;
    step
    stress(i)
    %
    if step >= 12000
        plot_step = 1;
        
        waitforbuttonpress
    end
    % pause(0.1)
    end
end
%alive
