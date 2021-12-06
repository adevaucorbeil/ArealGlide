function [CRSS,x,y]=extract_stable_configuration(filename,stepleap,doplot,dosave)

tic
mkdir('pics'); % Creates the directory where the figures are going to be saved.


%filename = 'data/1set-addfmv35deg-1';
%!rm pics/*
%fid = fopen([filename,'.out']);

if doplot==1
hf=figure('Position',[10 10 1000 500]);
hold on
end


% Read the file containing the obstacle distribution:

fid = fopen([filename,'.xy0']);
fid2 = fopen([filename,'.out']);

x(1)=0.2;
x(2)=0.5;
x(3)=0.75;
y(1)=0;
y(2)=0;
y(3)=0;

i=3;
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


i = 0;
j = 0;

x0=x;
y0=y;

seglength =[];
segangle =[];

while 1
    i = i+1;
    tline = fgetl(fid2);
    
    if ischar(tline)
        if isempty(find(tline=='B',1))
            CRSS_temp = str2num(tline(find(tline=='S',1)+2:find(tline=='T',1)-1));
            brokennode(i)=0;
        else
            CRSS_temp = str2num(tline(find(tline=='S',1)+2:find(tline=='B',1)-1));
            brokennode(i) = str2num(tline(find(tline=='k',1)+2:find(tline=='T',1)-1))+1;
            y(brokennode(i))=-1;
        end
    
        if exist('CRSS','var')
            CRSS(i) = max([CRSS CRSS_temp]);
        else
            CRSS(i) = CRSS_temp;
        end
    if i==1 || i/stepleap == floor(i/stepleap) || i>stepleap
            j=j+1;
            steps(j)=i;
            temp1 = str2num(tline(1:find(tline=='C',1)-1));
            
            e_pos = find(tline=='e'); 
            temp2 = str2num(tline(e_pos(end)+1:end));
            dislo = temp1(2:end)+1;
            segtype =zeros(1,length(dislo)-1);
            
            %[seglengthtemp,segangletemp]=seglengthangle(x0,y0,dislo,segtype);
            
            %seglength = [seglength,seglengthtemp];
            %segangle = [segangle,segangletemp];
            
            if CRSS_temp == max(CRSS)
                ys = y;
                dislo_s = dislo;
                segtype_s = segtype;
            end
            
            for k=1:length(temp2)/2
                segtype(temp2(2*k-1)+1) = temp2(2*k);
            end
            if doplot==1
            if exist('h1','var')
                cla(h1)
            end
            title(['Step #',num2str(i),' - ',num2str(length(dislo)-1),' obs pinned']);
            h1 = subplot(1,2,1);
            plot(x,y,'.');
            plotdislo(x,y,dislo,segtype);
            hold on
            axis([0 1 0 1])
            title(filename);
            xlabel('x');
            ylabel('y');
            hold on
            drawnow
            %waitforbuttonpress
            pause(0.1)
            
            if exist('h2','var')
                cla(h2)
            end
            h2 = subplot(1,2,2);
            plot(CRSS,'-k');
            xlabel('Step');
            ylabel('Maximum applied shear stress');
            hold on
            drawnow
            %waitforbuttonpress
            if dosave==1
                num = num2str(i+1000000);
                filenameimg = ['pics/',filename(find(filename == '/',1,'last')+1:end),'-',num(2:end),'.jpg'];
                saveas(hf,filenameimg);
            end
            end
        end
    else
        break
    end
end
fprintf(1,['The step that requires the maximum stress is :',num2str(find(CRSS==max(CRSS),1)),' with CRSS=',num2str(max(CRSS)),'\n']);
% Extract information from the critical step (step that requires the
% highest stress):

xs =x;

i = 0;
j = 0;

fclose all;

% imwrite(im,map,filename,'DelayTime',0,'LoopCount',inf);
fclose all; 
%close(hf);

% Optimise the size of the gif file (linux):
%fprintf(1,'Gif size optimisation\n');
%!convert pics/*.jpg -antialias -layers OptimizeTransparency +map pics/anim.gif
toc

end
