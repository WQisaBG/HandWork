import numpy as np
import matplotlib.pyplot as plt
import rclpy
from rclpy.node import Node
import rosbag2_py

class BagReader(Node):
    def __init__(self, bag_file):
        super().__init__('bag_reader')
        self.reader = rosbag2_py.SequentialReader()
        storage_options = rosbag2_py.StorageOptions(uri=bag_file, storage_id='mcap')
        converter_options = rosbag2_py.ConverterOptions()
        self.reader.open(storage_options, converter_options)

        self.data = []  # 存储接收到的数据
        while self.reader.has_next():
            topic, msg, t = self.reader.read_next()
            if topic == "/motor_command":  # 替换为您感兴趣的主题
                self.data.append(msg.data)  # 假设您要捕获的数据存储在msg.data中

        self.data = np.array(self.data)  # 转换为NumPy数组
        self.get_logger().info(f"Read {len(self.data)} messages from {topic}.")
        self.plot_data(self.data)  # 调用绘图函数

    def plot_data(self, data):
        plt.figure(figsize=(10, 6))
        plt.plot(data, marker='o', linestyle='-')  # 绘制加点的折线图
        plt.title("Data from ROS 2 Bag")
        plt.xlabel("Sample Index")
        plt.ylabel("Data Value")
        plt.grid(True)
        plt.show()

def main(args=None):
    rclpy.init(args=args)
    bag_reader = BagReader('/home/agvcore/HandWork/Tests/motor_command.bag')  # 替换为您实际的bag文件路径
    rclpy.spin(bag_reader)
    bag_reader.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
