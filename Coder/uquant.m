function num = uquant(max, bits, qNum)

    N = length(qNum);
    
    intMax = fix(max);
    
    intBits = round(log2(intMax));
    decBits = bits - intBits - 1;
    
    numTmp = zeros(N, 1);    
    
    for n = 1 : N
       
        numTmp(n) = qNum(n) / 2 ^ decBits;
        
    end    
    
    num = numTmp;    
    
end