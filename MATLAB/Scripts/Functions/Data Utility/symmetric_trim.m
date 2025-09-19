function output_array = symmetric_trim(input_array, n)
% Removes n samples from the start and the end of every row in input_array

output_array = input_array(n+1:end-n, :);

end

