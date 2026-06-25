clear all, close all, clc

s = serialport('COM6',9600)
pause(1);
configureTerminator(s, "LF");

N = 100;
data = zeros(N,1);

for i = 1:N
    disp(i)
    data(i) = double(readline(s))/10;
    pause(1)
end

figure
plot(data)

