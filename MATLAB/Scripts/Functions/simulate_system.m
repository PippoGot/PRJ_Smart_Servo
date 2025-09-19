function datasim = simulate_system(sim_params, block_params)
%SIMULATE_SYSTEM Summary of this function goes here
%   Detailed explanation goes here

%% Block Path Variables
reference_path = strcat(sim_params.filepath, '/Reference');
controller_path = strcat(sim_params.filepath, '/Controller');
driver_model_path = strcat(sim_params.filepath, '/Driver Model');
motor_model_path = strcat(sim_params.filepath, '/Motor Model');
sensor_array_path = strcat(sim_params.filepath, '/Sensor Models');
position_sensor_model_path = strcat(sensor_array_path, '/Position Sensor');
velocity_sensor_model_path = strcat(sensor_array_path, '/Velocity Sensor');
current_sensor_model_path = strcat(sensor_array_path, '/Current Sensor');
voltage_sensor_model_path = strcat(sensor_array_path, '/Voltage Sensor');
observer_path = strcat(sim_params.filepath, '/Observer');

%% Update Blocks

% Reference Signal
if strcmp(block_params.ref.model_type, 'step')
    set_param(reference_path, 'ReferencedSubsystem', "Step_Reference");
    set_param(reference_path, 'After', string(block_params.ref.value));
    set_param(reference_path, 'Time', string(block_params.ref.Tstep));
elseif strcmp(block_params.ref.model_type, 'sine')
    set_param(reference_path, 'ReferencedSubsystem', "Sine_Reference");
    set_param(reference_path, 'Frequency', string(block_params.ref.frequency));
    set_param(reference_path, 'Amplitude', string(block_params.ref.amplitude));
    set_param(reference_path, 'Value', string(block_params.ref.bias)); 
elseif strcmp(block_params.ref.model_type, 'triangle')
    set_param(reference_path, 'ReferencedSubsystem', "Triangle_Reference");
    set_param(reference_path, 'Freq', string(block_params.ref.frequency));
    set_param(reference_path, 'Gain', string(block_params.ref.amplitude));
    set_param(reference_path, 'Value', string(block_params.ref.bias)); 
elseif strcmp(block_params.ref.model_type, 'square')
    set_param(reference_path, 'ReferencedSubsystem', "Square_Reference");
    set_param(reference_path, 'Frequency', string(block_params.ref.frequency));
    set_param(reference_path, 'Amplitude', string(block_params.ref.amplitude));
    set_param(reference_path, 'Value', string(block_params.ref.bias)); 
elseif strcmp(block_params.ref.model_type, 'sequence')
    set_param(reference_path, 'ReferencedSubsystem', "Sequence_Reference");
    set_param(reference_path, 'OutValues', mat2str(block_params.ref.values));
    set_param(reference_path, 'tsamp', string(block_params.ref.Tint));
else
    set_param(reference_path, 'ReferencedSubsystem', "Step_Reference");
    set_param(reference_path, 'After', "0");
    set_param(reference_path, 'Time', string(block_params.ref.Tstep));
end


% Controller Strategy
if strcmp(block_params.controller.strategy, 'PID-position')
    set_param(controller_path, 'ReferencedSubsystem', "PID_Position");
    set_param(controller_path, 'P', string(block_params.controller.Kp));
    set_param(controller_path, 'I', string(block_params.controller.Ki));
    set_param(controller_path, 'D', string(block_params.controller.Kd));
elseif strcmp(block_params.controller.strategy, 'SWPID-position')
    set_param(controller_path, 'ReferencedSubsystem', "SWPID_Position");
    set_param(controller_path, 'Kp', string(block_params.controller.Kp));
    set_param(controller_path, 'Ki', string(block_params.controller.Ki));
    set_param(controller_path, 'Kd', string(block_params.controller.Kd));
elseif strcmp(block_params.controller.strategy, 'PID-velocity')
    set_param(controller_path, 'ReferencedSubsystem', "PID_Velocity");
    set_param(controller_path, 'P', string(block_params.controller.Kp));
    set_param(controller_path, 'I', string(block_params.controller.Ki));
    set_param(controller_path, 'D', string(block_params.controller.Kd));
elseif strcmp(block_params.controller.strategy, 'PID-current')
    set_param(controller_path, 'ReferencedSubsystem', "PID_Current");
    set_param(controller_path, 'P', string(block_params.controller.Kp));
    set_param(controller_path, 'I', string(block_params.controller.Ki));
    set_param(controller_path, 'D', string(block_params.controller.Kd));
elseif strcmp(block_params.controller.strategy, 'OPAMP-velocity')
    set_param(controller_path, 'ReferencedSubsystem', "OPAMP_Velocity");
else
    set_param(controller_path, 'ReferencedSubsystem', "No_Controller");
end

% Driver Model
if strcmp(block_params.system.driver_model, 'ideal')
    set_param(driver_model_path, 'ReferencedSubsystem', "Ideal_Driver");
elseif strcmp(block_params.system.driver_model, 'DRV8830')
    set_param(driver_model_path, 'ReferencedSubsystem', "DRV8830_Driver");
end
set_param(driver_model_path, 'driver_model', "DRV8830");


% Motor Model
if strcmp(block_params.system.motor_model, 'modular')
    set_param(motor_model_path, 'ReferencedSubsystem', "Modular_Motor");
    set_param(motor_model_path, 'initial_conditions', mat2str(block_params.system.initial_conditions));
    set_param(motor_model_path, 'flux_weakening', block_params.system.flux_weakening);
    set_param(motor_model_path, 'coulomb_friction', block_params.system.coulomb_friction);
    set_param(motor_model_path, 'stribeck_friction', block_params.system.stribeck_friction);
else
    set_param(motor_model_path, 'ReferencedSubsystem', "Reference_Motor");
end
set_param(motor_model_path, 'motor_model', "motor");


% Sensor Models
if strcmp(block_params.sensors.a_model, 'ideal')
    set_param(position_sensor_model_path, 'ReferencedSubsystem', "Ideal_Sensor");
    set_param(sensor_array_path, 'input1', "1");
elseif strcmp(block_params.sensors.a_model, 'AS5600')
    set_param(position_sensor_model_path, 'ReferencedSubsystem', "AS5600_Sensor");
    set_param(position_sensor_model_path, 'sensor_model', "AS5600");
    set_param(position_sensor_model_path, 'wrapping', "off");
    set_param(position_sensor_model_path, 'offset', "0");
else
    set_param(position_sensor_model_path, 'ReferencedSubsystem', "No_Sensor");
end

if strcmp(block_params.sensors.w_model, 'ideal')
    set_param(velocity_sensor_model_path, 'ReferencedSubsystem', "Ideal_Sensor");
    set_param(sensor_array_path, 'input2', "2");
elseif strcmp(block_params.sensors.w_model, 'filter')
    set_param(velocity_sensor_model_path, 'ReferencedSubsystem', "Real_Derivative");
    set_param(sensor_array_path, 'input2', "1");
else
    set_param(velocity_sensor_model_path, 'ReferencedSubsystem', "No_Sensor");
end

if strcmp(block_params.sensors.i_model, 'ideal')
    set_param(current_sensor_model_path, 'ReferencedSubsystem', "Ideal_Sensor");
    set_param(sensor_array_path, 'input3', "3");
elseif strcmp(block_params.sensors.i_model, 'INA219')
    set_param(current_sensor_model_path, 'ReferencedSubsystem', "INA219_Current_Sensor");
    set_param(sensor_array_path, 'input3', "3");
else
    set_param(current_sensor_model_path, 'ReferencedSubsystem', "No_Sensor");
end

if strcmp(block_params.sensors.v_model, 'ideal')
    set_param(voltage_sensor_model_path, 'ReferencedSubsystem', "Ideal_Sensor");
    set_param(sensor_array_path, 'input4', "4");
elseif strcmp(block_params.sensors.v_model, 'INA219')
    set_param(voltage_sensor_model_path, 'ReferencedSubsystem', "INA219_Voltage_Sensor");
    set_param(sensor_array_path, 'input4', "[3, 4]");
else
    set_param(voltage_sensor_model_path, 'ReferencedSubsystem', "No_Sensor");
end


% Observer Strategy
set_param(observer_path, 'ReferencedSubsystem', "No_Observer");

%% Simulate and Return Data

save_system(sim_params.filepath, 'SaveDirtyReferencedModels','on');
sim(sim_params.filepath);

datasim.time = sim_time;
datasim.ref = sim_reference;
datasim.cmd = sim_command;
datasim.t_voltage = sim_true_voltage;
datasim.t_state = sim_true_state;
datasim.t_torque = sim_true_torques;
datasim.m_voltage = sim_measured_voltage;
datasim.m_state = sim_measured_state;
datasim.e_state = sim_estimated_state;


end

