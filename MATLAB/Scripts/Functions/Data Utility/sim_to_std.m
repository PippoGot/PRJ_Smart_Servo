function [real_output_dataset, measured_output_dataset] = sim_to_std(dataset)
%EXP_TO_STD Summary of this function goes here
%   Packs simulation data in a standard format for use in other scripts.
%   Timeseries data is stored in column array.

real_output_dataset.position = dataset.t_state(:, 1);
real_output_dataset.velocity = dataset.t_state(:, 2);
real_output_dataset.current = dataset.t_state(:, 3);

real_output_dataset.voltage = dataset.t_voltage;

real_output_dataset.reference = dataset.ref;
real_output_dataset.time = dataset.time;


measured_output_dataset.position = dataset.m_state(:, 1);
measured_output_dataset.velocity = dataset.m_state(:, 2);
measured_output_dataset.current = dataset.m_state(:, 3);

measured_output_dataset.voltage = dataset.m_voltage;

measured_output_dataset.reference = dataset.ref;
measured_output_dataset.time = dataset.time;

end

