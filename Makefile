# Tên trình biên dịch và cờ
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Thư viện cần link
LIBS = -lsqlite3 -lssl -lcrypto

# Thư mục chứa mã nguồn
SRC_DIR = src
OBJS = $(SRC_DIR)/admin_utils.o $(SRC_DIR)/auth.o $(SRC_DIR)/backup_utils.o \
       $(SRC_DIR)/db_utils.o $(SRC_DIR)/main.o $(SRC_DIR)/otp.o \
       $(SRC_DIR)/transaction.o $(SRC_DIR)/user.o $(SRC_DIR)/wallet.o

# Tên chương trình xuất ra
TARGET = reward_wallet_system

# Mục mặc định
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Làm sạch file .o và file thực thi
clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)

# Tùy chọn rebuild
rebuild: clean all
