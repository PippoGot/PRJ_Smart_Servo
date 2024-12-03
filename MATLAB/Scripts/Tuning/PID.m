%% Transfer Functions

s = tf('s');

TF.El = 1/(motor.La*s + motor.Ra);
TF.Me = 1/(motor.Je*s + motor.Be);

TF.Br = 5;

TF.Kphi = motor.Kphi;
TF.Kg = motor.Kg;


%% Current loop

% Functions
Current.OL = TF.Br * feedback(TF.El, TF.Kphi^2 * TF.Me);
Current.FB = 1;
Current.L0 = Current.OL*Current.FB;

[num_curr, den_curr] = tfdata(Current.OL, 'v');

% Specifications
Current.tr = 1e-3;              % rise time     [s]
Current.wb = 600;  % bandwidth     [rad/s]
Current.mphi = deg2rad(90);     % phase margin  [rad]

% Tuning
[Current.Mag, Current.Phase] = bode(Current.L0, Current.wb);
Current.Phase = deg2rad(Current.Phase);

Current.Tpi = tan(Current.mphi - 2*pi - Current.Phase) / Current.wb;


Current.L1 = (1 + s*Current.Tpi)*Current.L0;


[Current.Mag, Current.Phase] = bode(Current.L1, Current.wb);
Current.Ki = 1 / Current.Mag;
Current.Kp = Current.Ki * Current.Tpi;

% Loop Function
Current.PI = Current.Ki / s + Current.Kp;
Current.CL = feedback(Current.PI*Current.OL, Current.FB);

% Visualization
% figure(1);
% bode(Current.OL, 'r', Current.CL, 'b');


%% Speed Loop

% Functions
Speed.OL = Current.CL + TF.Me * TF.Kphi * TF.Kg;
Speed.FB = (wn^2*s)/(s^2 + 2*csi*wn + wn^2);
Speed.L0 = Speed.OL*Speed.FB;

% Specifications
Speed.tr = 1e-5;            % rise time     [s]
Speed.wb = 300;  % bandwidth     [rad/s]
Speed.mphi = deg2rad(90);   % phase margin  [rad]

% Tuning
[Speed.Mag, Speed.Phase] = bode(Speed.L0, Speed.wb);
Speed.Phase = deg2rad(Speed.Phase);

Speed.Tpi = tan(Speed.mphi - 2*pi - Speed.Phase) / Speed.wb;


Speed.L1 = (1 + s*Speed.Tpi)*Speed.L0;


[Speed.Mag, Speed.Phase] = bode(Speed.L1, Speed.wb);
Speed.Ki = 1 / Speed.Mag;
Speed.Kp = Speed.Ki * Speed.Tpi;

% Loop Function
Speed.PI = Speed.Ki / s + Speed.Kp;
Speed.CL = feedback(Speed.PI*Speed.OL, Speed.FB);

% Visualization
% figure(2);
% bode(Speed.OL, 'r', Speed.CL, 'b');
