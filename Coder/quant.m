function qNum = quant(max, bits, num)

    N = length(num);
    
    intMax = fix(max);
    
    intBits = round(log2(intMax));
    decBits = bits - intBits - 1;
    
    qNumTmp = zeros(N, 1);
    
    
    for n = 1 : N
       
        qNumTmp(n) = round(num(n) * 2 ^ decBits);
        
    end    
    
    qNum = qNumTmp;    
    
end