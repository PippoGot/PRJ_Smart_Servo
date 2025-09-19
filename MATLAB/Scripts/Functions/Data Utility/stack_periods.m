function output_array = stack_periods(input_array, N)
% Divide a signal (row) array in a column of N (row) arrays, where N is the
% number of periods. If the input array has a number of elements that
% is not divisible by N, the n remainder last elements will be ignored.

% EXAMPLE:
% size(input_array) = [1, 100001]
% N = 20
% size(output_array) = [20, 5000]
% the last element is neglected

samples_per_period = floor(size(input_array, 2) / N);

temp_array = zeros(N, samples_per_period);
j = 1;
for i = 1:1:N
    l = i*samples_per_period;
    temp_array(i, 1:samples_per_period) = input_array(j:l);
    j = j + samples_per_period;
end

output_array = temp_array;

end

