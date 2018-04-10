function randu=randu(iseed,n)

randu=zeros(n,1);

imax = 2^31-1;
xmax_inv = 1/imax;

for i=1:n
    iseed = iseed*65539;

    iseed = mod(iseed,imax+1);
    randu(i) = iseed;%*xmax_inv;
end

end