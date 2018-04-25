filename = 'testA.txt';
filename2 = 'testA.txt';

FID = fopen(filename);
dataFromfile = textscan(FID,'%n');
decData = cell2mat(dataFromfile);
fclose(FID);

fs = 10000;
samples = 1000;
t = 1/fs *linspace(0, samples -1/fs,samples);
f = fs * linspace(0, 1, samples);

xn = decData;

%figure(1);
subplot(2,2,1);
plot(t,xn);
title('Respuesta temporal conexion de cobre y optica');
xlabel('Tiempo');
ylabel('Amplitud');

XF = fft(xn);
%XF2 = fft(xn2);

subplot(2,2,2);
stem(f,20*log10(abs(XF)),'filled');
xlim([0,samples]);
title('Respuesta en frecuencia conexion de cobre');
xlabel('Frecuencia');
ylabel('Amplitud');
