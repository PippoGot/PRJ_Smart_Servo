function hexStr = toHexString(num)
%#codegen
chars = '0123456789ABCDEF';
hi = floor(num/16);
lo = mod(num,16);
hexStr = ['0x', chars(hi+1), chars(lo+1)];
end
