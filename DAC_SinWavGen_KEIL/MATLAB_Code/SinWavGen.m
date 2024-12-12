%% Sin Wave Generator : f_signal = 1 KHz -> fs_min = 2 KHz -> fs = 10 KHz
% By Diba Falati

clear all;
close all;
clc;

f_signal = 1000;
fs = 10*f_signal;

t_signal = 1/f_signal;
ts = 1 / fs;

n = t_signal / ts;

A = 1;
t = 0 : 2*pi/n : 2*pi;
offset = A;

y = A * sin(t) + offset;
plot(t, y)

y_2 = (y * 4096) / 3.3;