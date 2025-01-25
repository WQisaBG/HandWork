import numpy as np
import matplotlib.pyplot as plt
import rclpy
from rclpy.node import Node
import rosbag2_py

class MultiBagReader(Node):
    def __init__(self, bag_files, topic):
        super().__init__('multi_bag_reader')
        self.topic = topic
        self.all_data = []  # 用于存储所有 bag 数据
        self.file_names = []  # 用于存储 bag 文件名称

        for bag_file in bag_files:
            self.file_names.append(bag_file)
            self.read_bag_file(bag_file)

        self.plot_data(self.all_data)  # 一次性绘制所有数据

    def read_bag_file(self, bag_file):
        self.get_logger().info(f"Reading from {bag_file}")
        reader = rosbag2_py.SequentialReader()
        storage_options = rosbag2_py.StorageOptions(uri=bag_file, storage_id='sqlite3')
        converter_options = rosbag2_py.ConverterOptions()
        reader.open(storage_options, converter_options)

        data = []  # 每个 bag 文件的数据
        while reader.has_next():
            topic, msg, t = reader.read_next()
            if topic == self.topic:  # 只提取感兴趣的主题
                data.append(msg.data)  # 假设 msg.data 是您要的值

        self.all_data.append(np.array(data))  # 将数据存储为 NumPy 数组
        self.get_logger().info(f"Read {len(data)} messages from {bag_file}")

    def plot_data(self, all_data):
        plt.figure(figsize=(10, 6))
        for i, data in enumerate(all_data):
            plt.plot(data, marker='o', linestyle='-', label=f'File {self.file_names[i]}')  # 绘制每个文件的数据
        plt.title("Data from Multiple ROS 2 Bags")
        plt.xlabel("Sample Index")
        plt.ylabel("Data Value")
        plt.legend()
        plt.grid(True)
        plt.show()

def main(args=None):
    rclpy.init(args=args)

    # 替换为您要读取的 bag 文件路径和主题名称
    bag_files = ['path_to_bag_file_1.bag', 'path_to_bag_file_2.bag']  # 这里填入您要读取的 bag 文件路径
    topic = '/your_topic'  # 替换为感兴趣的主题

    multi_bag_reader = MultiBagReader(bag_files, topic)
    rclpy.spin(multi_bag_reader)

    multi_bag_reader.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
