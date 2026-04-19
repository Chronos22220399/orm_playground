import re

with open('include/ess/orm/sql/details/parser_common.ipp', 'r') as f:
    content = f.read()

# 查找所有字面量检查的模式
pattern = r'(} else if \(peek\(\)\.type == TokenType::Number\s*\|\|\s*peek\(\)\.type == TokenType::String\s*\|\|\s*peek\(\)\.type == TokenType::PlaceHolder\s*\|\|\s*peek\(\)\.type == TokenType::True\s*\|\|\s*peek\(\)\.type == TokenType::False\s*\|\|\s*peek\(\)\.type == TokenType::Null\) \{[^{}]*// Literal value)'

# 替换函数
def replace_literal(match):
    return match.group(1) + '\n    result.has_literal = true;  // 标记有字面量'

new_content = re.sub(pattern, replace_literal, content)

with open('include/ess/orm/sql/details/parser_common.ipp', 'w') as f:
    f.write(new_content)

print("修改完成")
