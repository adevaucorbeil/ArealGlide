function mean_dist=neighbours(filename)

%filename='cluster-10000obs-5deg-80percent_cores-maxradius0.02';
%filename='cluster-10000obs-5deg-10percent_cores-maxradius0.001';

fid = fopen([filename,'.xy0']);

i = 0;
j = 0;
while 1
    i = i+1;
    tline = fgetl(fid); 
    if ischar(tline)
    
        point=str2num(tline);
        x(i,1)=point(1);
        y(i,1)=point(2);
        
    else
        break
    end
end
    
fclose(fid);

m=length(x);

% Compute the first neighbours:

% Take a random point:
%figure

dist_add = zeros(1,m);

n=10000;
for i=1:n
%pt = floor(rand()*m);
pt = n;
% Compute the distances in between this random chosen point and all the
% other points:

dist_center = sqrt((x(pt) - x).^2 + (y(pt) - y).^2);
dist_left_cell = sqrt((x(pt) - (x-1)).^2 + (y(pt) - y).^2);
dist_right_cell = sqrt((x(pt) - (x+1)).^2 + (y(pt) - y).^2);

dist = sort(min([dist_center'; dist_left_cell'; dist_right_cell']));

dist_add = dist_add + dist;

%plot(dist,'-');
%hold on
end

mean_dist = dist_add/n;

%plot(dist_mean,'-d');