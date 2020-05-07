import datetime
import numpy as np

#read from thread function

def process_data(my_thread):
    session = SkiSession(0, 0, 0, datatime.datetime.now())
    while(len(my_thread.in_data)>0): 
        data = my_thread.read_data_in()
        data = data.split(",")
        outputs = []
        tail = data[0]
        for entry in data[1:]: 
            if (delims.find(tail) != -1): 
                outputs.append(float(entry))
            tail = entry
        temp = session.add_gx_entry(outputs[0], outputs[1], outputs[2] , datetime.datetime.now())
    for (entry in session.get_speed()): 
        my_thread.write_data_out(my_thread.speed_format_str + string(entry))
    
                
                
     #only process accelerometer 
    
    
        



class SkiSession():
    instances = []
    time_threshold = 3600;

    def __init__(self, x, y, z, date_begin):
        # look for sessions to be defined as an end in constant acceleration
        self.raw_data_array = []
        self.gyro_data = []
        first_raw_data = raw_data(x, y, z, 0)
        self.raw_data_array.append(first_raw_data)
        SkiSession.instances.append(self)
        self.date_begin = date_begin
        self.orientation_data = []
        self.interpolated_orientation = []
        self.current_interpolation_len = 0
        self.current_gravity_adjustment = 0
        self.speed_array = []
        self.displacement_array = []
    def add_gx_entry(self, theta_r, omega_r, new_date):
        diff_time = new_date
        diff_time = new_date - self.date_begin
        diff_time = diff_time.total_seconds()
        diff_time = float(diff_time)
        assert diff_time<SkiSession.threshold
        self.gyro_data.append(GyroData(theta_r, omega_r, diff_time))
        return self
    def add_ax_entry(self, x, y, z, new_date):
        diff_time = new_date - self.date_begin
        diff_time = diff_time.total_seconds()
        diff_time = float(diff_time)
        if diff_time > SkiSession.time_threshold:
            return SkiSession(x, y, z, new_date)
        else:
            self.raw_data_array.append(raw_data(x, y, z, diff_time))
        return self
    def set_orientation_data(self):
        if len(self.orientation_data) == 0:
            self.orientation_data.append(GyroData(0, 0, self.gyro_data[0].delta_time))
        while(len(self.orientation_data)<len(self.gyro_data)):
            self.orientation_data.append(self.orientation_data[-1] + self.gyro_data(len(self.orientation_data)))
        return self
    def adjust_for_gravity(self):
        """
        Uses gyroscope data to
        Note*
        Gyroscope Data is sampled at constant rate from user device using given API
        Interpolation can be used using fft sync interpolation for this reason

        """
        new_sample_time = 20 # hertz
        last_time = self.orientation_data[self.current_interpolation_len].delta_time
        current_time = self.orientation_data[-1].delta_time
        target_len = (last_time - current_time)*new_sample_time
        to_interpolate = self.orientation_data[self.current_interpolation_len:]
        theta_r = [guy.theta_r for guy in to_interpolate]
        omega_r = [guy.omega_r for guy in to_interpolate]
        theta_r = sync_interp(theta_r, target_len)
        omega_r = sync_interp(omega_r, target_len)
        count = 0
        for item in self.raw_data_array[self.current_gravity_adjustment:]:
            if (item.delta_time>last_time):
                break
            else:
                count = count + 1
            index = (last_time - item.delta_time)*new_sample_time
            index = np.floor(index)
            item.adjust_for_gravity(theta_r[index], omega_r[index])
        self.current_gravity_adjustment = count + self.current_gravity_adjustment
    def get_speed(self):
        self.adjust_for_gravity()
        if (len(self.speed_array) == len(self.raw_data_array))
            return self.speed_array
        elif (len(self.speed_array) == self.current_gravity_adjustment)
            return self.speed_array
        elif (len(self.speed_array) == 0)
            self.speed_array.append(raw_data(0, 0, 0))
        tail = self.speed_array[-1]
        for item in self.raw_data_array[1:self.current_gravity_adjustment]:
            self.speed_array.append(item.add_a_to_s(tail, self.speed_array[-1]))
            tail = item
        return self.speed_array
    def get_displacement(self):
        self.get_speed()
        current_len = len(self.displacement_array)
        if current_len  == len(self.speed_array)
            return self.displacement_array
        elif current_len  == 0
            self.displacement_array.append(raw_data(0, 0, 0))
        tail = self.displacement_array[-1]
        for item in self.speed_array[current_len:]:
            self.displacement_array.append(item.add_d_to_s(tail, self.displacement_array[-1]))
            tail = item
        return self.displacement_array













def sync_interp(input_array, target_length):
    required_samples = target_length - len(input_array)
    temp_fft = np.fft.fft(input_array)
    fft_zero_padded = temp_fft + [0 for i in range(1, required_samples)]
    return np.fft.ifft(fft_zero_padded)








class SpeedData:
    def __int__(self, x, y, z, delta_time):
        self.x = x
        self.y = y
        self.z = z
        self.delta_time = delta_time

class GyroData:
    def __init__(self, theta_r, omega_r, delta_time):
        self.theta_r = (((theta_r / GyroData.max_value) - GyroData.zero_value)/GyroData.sensitivity)
        self.omega_r = (((omega_r / GyroData.max_value) - GyroData.zero_value) / GyroData.sensitivity)
        self.delta_time = delta_time
    def __add__ (self, other):
        width = abs(self.delta_time - other.delta_time)
        get_bigger = lambda j, k: j if (j>k) else k
        avg = lambda j, k : j+k/2
        theta = avg(self.theta_r, other.theta_r)
        omega = avg(self.omega_r, other.omega_r)
        return GyroData(theta, omega, get_bigger(self.delta_time, other.delta_time))


class raw_data:
    def __init__(self, *argv):
        assert len(argv) == 4 or len(argv) == 2
        if len(argv) == 3:
            self.x = argv[0]
            self.y = argv[1]
            self.z = argv[2]
            self.regular_vector = [argv[0], argv[1], argv[2]]
        else:
            self.regular_vector = argv[0]
            self.x = self.regular_vector[0]
            self.y = self.regular_vector[1]
            self.z = self.regular_vector[2]
        self.delta_time = argv[-1]
        self.adjusted_for_gravity = 0
        self.mag = self.get_mag()
        self.unit_vector = []
        self.update_unit_vector()
    def adjust_for_gravity(self, theta_r, omega_r):
        pre_unit_vector = self.regular_vector
        unit_vector = [0]*3
        unit_vector[0] = (pre_unit_vector[0]*np.cos(theta_r)) + (pre_unit_vector[2]*np.sin(theta_r))
        unit_vector[1] = (pre_unit_vector[2]*np.cos(theta_r)) - (pre_unit_vector[0]*np.sin(theta_r))
        unit_vector[1] = (pre_unit_vector[1] * np.cos(omega_r)) - (np.cos(omega_r) * unit_vector[0])
        unit_vector[2] = (pre_unit_vector[2]*np.cos(theta_r)) - (pre_unit_vector[0]*np.sin(theta_r))
        unit_vector[2] = (unit_vector[2] * np.cos(omega_r)) + (np.cos(omega_r) * unit_vector[1])
        unit_vector[2] = unit_vector[2] - 1 # subtract gravity
        self.regular_vector = unit_vector
        self.update_unit_vector()
        self.adjusted_for_gravity = 1
        return self
    def __add__(self, other):
        # trapazoidal integration
        values = []
        width = abs(self.delta_time - other.delta_time)
        for me, you in zip(self.regular_vector, other.regular_vector):
            values.append(np.average[me, you] * width)
        get_bigger = lambda j, k: j if (j > k) else k
        return raw_data(values, get_bigger(self.delta_time, other.delta_time))
    def a_to_s(self, other):
        temp = self + other
        temp.regular_vector = [one*9.8 for one in temp.regular_vector]
        return raw_data(temp.regular_vector, temp.delta_time)
    def add_a_to_s(self, other, speed):
        new = a_to_s(self, other)
        output = [one + two for one, two  in zip(new.regular_vector, speed.regular)]
        return raw_data(output, self.delta_time)
    def add_d_to_s(self, other, disp):
        new = self+other
        output = [one + two for one, two in zip(new.regular_vector, disp.regular)]
        return raw_data(output, self.delta_time)


    def get_mag(self):
        return (np.sum([item**2 for item in self.regular_vector]))**(1/2)
    def update_unit_vector(self):
        self.unit_vector = [item/self.get_mag() for item in self.regular_vector]







