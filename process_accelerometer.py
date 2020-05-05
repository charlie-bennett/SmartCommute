import datetime

#TODO find 0 points
class SkiSession():
    instances = []
    time_threshold = 10

    def __init__(self, x, y, z, date_begin):
        # look for sessions to be defined as an end in constant acceleration
        self.raw_data_array = []
        first_raw_data = raw_data(x, y, z, 0)
        self.raw_datas.append(first_raw_data)
        SkiSession.instances.append(self)
        self.date_begin = date_begin

    # def get_most_recent_date(self):
    #     return self.raw_datas[-1].date

    def add_entry(self, x, y, z, new_date):
        diff_time = new_date - self.date_begin
        diff_time = diff_time.total_seconds()
        diff_time = float(diff_time)
        if diff_time> SkiSession.time_threshold:
            return SkiSession(x, y, z, new_date)
        else:
            self.raw_datas.append(raw_data(x, y, z, diff_time))
            return self




class raw_data():
    def __init__(self, x, y, z,  delta_time):
        self.x = x
        self.y = y
        self.z = z
        delta_time = delta_time
#
# class xyz_accel()
#     instances = []
#     delims = ["", "", ""] # x, y, z
#     in_bt = ""
#
#     def __init__(self, line, current_time):
#         xyz_accel.instances.append(self)
#         self.current_time = current_time
#         line = line.rstrip()
#         line = line.decode("utf-8")
#         line = str(line)
#         count = 0
#         self.x = 0
#         self.y = 0
#         self.z = 0
#         self.speed = []
#         # self.mag = 0
#         # self.dir = []
#         # self.delta_t = 0
#         # delimit
#         for item in line.split(in_bt):
#             if tail == xyz_accel.delims[count]:
#                 if count == 0:
#                     self.x = float(item)
#                 elif count == 1:
#                     self.y = float(item)
#                 elif count == 2:
#                     self.z = float(item)
#                 count = count + 1
#             tail = item
#
#         # if (xyz_accel.instances.index(self) != 0):
#         #     last = xyz_accel.instances[-2]
#         #     self.delta_t = float(self.current_time - last.current_time)
#         #     self.speed = [(one - two)/self.delta_t for [one, two] in
#         #                   [[self.x, self.y, self.z], [last.x, last.y, last.z]]]
#         #     for entry in self.speed:
#         #         self.mag = self.mag + pow(entry, 2)
#         #     self.mag = pow(self.mag, .5)
#         #     self.dir = [me/self.mag for me in self.speed]
#
#
# def remove_constants(x):
#     # differentiation or high pass filtering
#     diff = []
#     tail = x[0]
#     for value in x[1:]:
#         diff = value - tail
#
#         diff = value









