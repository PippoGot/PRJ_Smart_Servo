function output_dataset = exp_to_std(dataset)
%EXP_TO_STD Summary of this function goes here
%   Packs experimental data in a standard format for use in other scripts.
%   Timeseries data is stored in column array.

output_dataset.position = reshape(get(dataset, 'position').Values.Data, [], 1);
output_dataset.velocity = reshape(get(dataset, 'velocity').Values.Data, [], 1);
output_dataset.current = reshape(get(dataset, 'raw_current').Values.Data, [], 1);

output_dataset.voltage = reshape(get(dataset, 'raw_voltage').Values.Data, [], 1);

output_dataset.reference = reshape(get(dataset, 'ref').Values.Data, [], 1);
output_dataset.time = 1e-3 * (0:1:size(output_dataset.position, 1) - 1)';

output_dataset.command = reshape(get(dataset, 'duty').Values.Data, [], 1);

end

