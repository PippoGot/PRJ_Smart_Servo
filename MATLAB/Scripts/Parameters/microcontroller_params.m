%% Microcontroller (from datasheet and design choices)

UC.pwr = 3.3;               % power supply                                  [V]
UC.Fclk = 64e6;             % internal clock frequency                      [Hz]
UC.Ftim = UC.Fclk/2;        % timer clock frequency                         [Hz]

UC.Ts = 1e-3;               % sampling time                                 [s]

% PWM Generation
UC.steps = 128;             % total number of duty cycle steps              [#]
UC.d_inc = 100 / UC.steps;  % smallest duty cycle variation                 [%]

UC.Fpwm = 500e3;            % PWM frequency                                 [Hz]
UC.Tpwm = 1 / UC.Fpwm;      % PWM period                                    [s]

UC.psc = floor(UC.Fclk/(UC.steps*UC.Fpwm)) - 1;     % prescaler             [uint8]

UC.Fpwmx = UC.Fclk/(UC.steps*(UC.psc + 1));         % actual PWM frequency  [Hz]
UC.Tpwmx = 1 / UC.Fpwmx;    % actual PWM period                             [s]
