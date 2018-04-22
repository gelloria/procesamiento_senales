function CoderCoeffFreq(file)

    [xn, fs] = audioread(file);     %read audio file and get signal and sample rate
    
    N = length(xn);      %total of samples
    
    M = 64;     %number of coefficients
    
    fileID = fopen('Wav.txt', 'w');      %open output file for original wav rfft
    fileID2 = fopen('Wav.qwv', 'w');      %open output file for quantized wav rfft
    fileID3 = fopen('uWav.txt', 'w');      %open output file for unquantized wav rfft
    fileID4 = fopen('qWav.txt', 'w');      %open output file for quantized wav rfft
    
    player = audioplayer(xn, fs)   %build player object for audio reproduction
    
    play(player);    
    pause(N/fs);
    
    F = round(N / M * (M + 2) / 2);
    qBits1 = 2;
    qBits2 = 8;
    qBits3 = 8;
    qBits4 = 4;
    
    xm = zeros(M,1);        %vector for M samples
    yn = zeros(N,1);        %output signal of original N samples
    XF = complex(zeros(F, 1), zeros(F, 1));
    XFF = complex(zeros((M + 2) / 2, 1), zeros((M + 2) / 2, 1));
    %rfft coefficients for f < 242Hz => 2 bits
    rXFM1 = zeros(1, 1);
    iXFM1 = zeros(1, 1);
    %rfft coefficients for 242Hz < f < 7272Hz  => 8 bits
    rXFM2 = zeros(29, 1);
    iXFM2 = zeros(29, 1);
    %rfft coefficients for 7272Hz < f < 7758Hz  => 4 bits
    rXFM3 = zeros(2, 1);
    iXFM3 = zeros(2, 1);
    %rfft coefficients for 7758Hz < f < 8000Hz  => 2 bits
    rXFM4 = zeros(1, 1);
    iXFM4 = zeros(1, 1);
    
    f = 1;
    Max = 0;
    %Loop for each sample in signal
    for n = 1:M:N
        
        nm = 1;
    
        %Loop for building vector of M samples
        for m = n:(n + M - 1)            
            xm(nm) = xn(m); 
            nm = nm + 1;
        end
        
        %Get real FFT of M coefficients. Output would be (M+2)/2
        XFM = complex(rfft(xm, M));
        
        %Write coefficcients to output file for original rFFT
        for fm=1:(M + 2)/2
            
           fprintf(fileID, '%f,%f\r\n', real(XFM(fm)), imag(XFM(fm)));  
           XF(f) = complex(XFM(fm));
           f = f + 1;
           
           if Max < abs(real(XFM(fm)))
               Max = abs(real(XFM(fm)));
           end
           
           if Max < abs(imag(XFM(fm)))
               Max = abs(imag(XFM(fm)));
           end         
           
        end 
        
    end    
    
    fprintf(fileID2, '%i\r\n', Max);
    
    n = 1;
    
    %Loop for each sample in signal
    for f = 1:(M + 2)/2:F
        
        %Write coefficcients to output file for original rFFT
        f0 = 1;
        f1 = 1;
        f2 = 1;
        f3 = 1;
        f4 = 1;
        for fm=f:f + (M + 2)/2 - 1
            
            XFM(f0) = XF(fm);
            
            if f0 < 2 
               rXFM1(f1) = real(XF(fm));
               iXFM1(f1) = imag(XF(fm));
               f1 = f1 + 1;              
           elseif f0 < 31
               rXFM2(f2) = real(XF(fm));
               iXFM2(f2) = imag(XF(fm));
               f2 = f2 + 1;              
           elseif f0 < 33
               rXFM3(f3) = real(XF(fm));
               iXFM3(f3) = imag(XF(fm));
               f3 = f3 + 1;              
           else
               rXFM4(f4) = real(XF(fm));
               iXFM4(f4) = imag(XF(fm));
               f4 = f4 + 1;              
           end   
           f0 = f0 + 1;
        end   
              
        
        rqXFM1 = quant(Max, qBits1, rXFM1);           %apply factors to original rFFT
        iqXFM1 = quant(Max, qBits1, iXFM1);           %apply factors to original rFFT
        rqXFM2 = quant(Max, qBits2, rXFM2);           %apply factors to original rFFT
        iqXFM2 = quant(Max, qBits2, iXFM2);           %apply factors to original rFFT
        rqXFM3 = quant(Max, qBits3, rXFM3);           %apply factors to original rFFT
        iqXFM3 = quant(Max, qBits3, iXFM3);           %apply factors to original rFFT
        rqXFM4 = quant(Max, qBits4, rXFM4);           %apply factors to original rFFT
        iqXFM4 = quant(Max, qBits4, iXFM4);           %apply factors to original rFFT
      
        
        rXFM1 = uquant(Max, qBits1, rqXFM1);
        iXFM1 = uquant(Max, qBits1, iqXFM1);
        rXFM2 = uquant(Max, qBits2, rqXFM2);
        iXFM2 = uquant(Max, qBits2, iqXFM2);
        rXFM3 = uquant(Max, qBits3, rqXFM3);
        iXFM3 = uquant(Max, qBits3, iqXFM3);
        rXFM4 = uquant(Max, qBits4, rqXFM4);
        iXFM4 = uquant(Max, qBits4, iqXFM4);
        
        
        %Write coefficcients to output file for original rFFT
        f1 = 1;
        f2 = 1;
        f3 = 1;
        f4 = 1;
        for fm=1:(M + 2)/2
           if fm < 2 
              XFF(fm) = complex(rXFM1(f1), iXFM1(f1));              
              fprintf(fileID4, '%d,%d\r\n', rqXFM1(f1), iqXFM1(f1));
              fwrite(fileID2,rqXFM1(f1),'int8');
              fwrite(fileID2,iqXFM1(f1),'int8');
              fprintf(fileID3, '%f,%f\r\n', rXFM1(f1), iXFM1(f1));
              f1 = f1 + 1;
           elseif fm < 31
              XFF(fm) = complex(rXFM2(f2), iXFM2(f2));
              fprintf(fileID4, '%d,%d\r\n', rqXFM2(f2), iqXFM2(f2));
              fwrite(fileID2,rqXFM2(f2),'int8');
              fwrite(fileID2,iqXFM2(f2),'int8');
              fprintf(fileID3, '%f,%f\r\n', rXFM2(f2), iXFM2(f2));
              f2 = f2 + 1;
           elseif fm < 33
              XFF(fm) = complex(rXFM3(f3), iXFM3(f3));
              fprintf(fileID4, '%d,%d\r\n', rqXFM3(f3), iqXFM3(f3));
              fwrite(fileID2,rqXFM3(f3),'int8');
              fwrite(fileID2,iqXFM3(f3),'int8');
              fprintf(fileID3, '%f,%f\r\n', rXFM3(f3), iXFM3(f3));
              f3 = f3 + 1;
           else
              XFF(fm) = complex(rXFM4(f4), iXFM4(f4));
              fprintf(fileID4, '%d,%d\r\n', rqXFM4(f4), iqXFM4(f4));
              fwrite(fileID2,rqXFM4(f4),'int8');
              fwrite(fileID2,iqXFM4(f4),'int8');
              fprintf(fileID3, '%f,%f\r\n', rXFM4(f4), iXFM4(f4));
              f4 = f4 + 1;
           end       
                     
        end
        
         %Get output signal of M samples using real IFFT
        ym = irfft(XFF, M);
        
        %Loop for building output vector of N samples
        for m = 1:M
            yn(n) = ym(m);
            n = n + 1;
        end
        
    end
    
    fclose(fileID);         %close output file
    fclose(fileID2);         %close output file
    fclose(fileID3);         %close output file
    fclose(fileID4);         %close output file
    
    player = audioplayer(yn, fs)    %build player object for audio reproduction
    play(player);                   %play output signal
    
    pause(N/fs);                    %pause time of N / fs long

end