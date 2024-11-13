NAME            =   webserv

CXX             =   c++
CXXFLAGS        =   -Wall -Wextra -std=c++98 -MMD -MP -I.

BUILD_DIR       =   __build__
SRC_DIR         =   srcs

SRC             =   $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJ             =   $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEP             =   $(OBJ:.o=.d)

RM              =   rm -rf

all: $(NAME)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

-include $(DEP)

clean:
	$(RM) $(BUILD_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
.SECONDARY: $(OBJ)
