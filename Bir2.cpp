#include <iostream>
#include "../Project0/Declarations.h"

int main(int argc, char* argv[])
{
	QFile output_file(argv[3]);
	output_file.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream out(&output_file);

	if (argc != 4) {
		out << "Too many arguments." << endl;
		return -1;
	}

	Node* tree = new Node;
	tree->set_type(operation);

	QString error_text = "";
	switch (Read_tree_from_file(argv[2], *tree))
	{
	case -1:
		error_text = "Invalid input file specified. The file may not exist.";
	case -2:
		error_text = "The file extension is incorrect. The file must have the extension .xml";
	case -3:
		error_text = "The file can not be read. The input file may not have the correct syntax.";
	case -4:
		error_text = "The source tree contains unsupported operations.";
	case -5:
		error_text = "The length of one or more operands exceeds the maximum length (100 characters).";
	case -6:
		error_text = "No comparison signs were encountered in the input file.";
	case -7:
		error_text = "Incorrect location of the comparison operation in the input tree.";
	case -8:
		error_text = "One or more operations correspond to an incorrect number of operands.";
	default:

		// если не удалось создать выходной файл
		//вывод ?куда? "Invalid output file specified. The specified location may not exist.";
		//иначе:
		out << error_text;

	

		break;
	}

	Transfering_to_left_side(*tree);


	Node* ghost_parent = new Node;
	ghost_parent->set_type(operation);
	ghost_parent->set_value("");
	std::vector<Node*> ghost_parent_child = { tree };
	ghost_parent->set_children(ghost_parent_child);

	Uniting_pluses_minuses_multiplications(*tree, *ghost_parent);


	Sorting_in_alphabet_order(*tree->get_children()[0]);

	Write_nodes(out, *tree);

	output_file.close();
	return 0;

	/*	int count = 0;
		while (!reader.atEnd())
		{
			reader.readNext();
			if (reader.hasError())
			{
				out << "Errors in xml." << endl;
				return -2;
			}
			if (reader.isStartElement())
			{
				count++;
			}
		}*/
}

int Read_tree_from_file(char file_path[], Node& tree)
{
	QFile* input_file = new QFile(file_path);

	if (!input_file->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return -1;
	}

	QXmlStreamReader reader(input_file);

	reader.readNextStartElement();
	Read_nodes(reader, tree);

	tree = *tree.get_children()[0];

	input_file->close();
	return 0;
}

int Read_nodes(QXmlStreamReader& reader, Node& parent_node)
{
	Node* current_node = new Node;

	if (reader.name() == "operation")
	{
		current_node->set_type(operation);
		current_node->set_value(reader.attributes().value("symbol").toString());
		parent_node.add_child(current_node);

		while (true)
		{
			reader.readNext();
			while (!reader.isStartElement() && !reader.atEnd())
			{
				reader.readNext();
				if (reader.isEndElement() && reader.name() == "operation")
				{
					return 0;
				}
			}
			if (reader.atEnd())
			{
				return -1;
			}
			Read_nodes(reader, *current_node);
		}
	}
	else
	{
		if (reader.name() == "operand")
		{
			current_node->set_type(operand);
			current_node->set_value(reader.readElementText());

			parent_node.add_child(current_node);

			return 0;
		}
	}
}

void Transfering_to_left_side(Node& main_node)
{
	if (main_node.get_children()[1]->get_value() != "0")
	{
		Node* zero_node = new Node;
		zero_node->set_value("0");
		zero_node->set_type(operand);

		Node* plus_node = new Node;
		plus_node->set_value("+");
		plus_node->set_type(operation);
		plus_node->add_child(main_node.get_children()[0]);
		plus_node->add_child(main_node.get_children()[1]);

		Change_sign(*plus_node->get_children()[1], *plus_node, 1);

		main_node.change_child(plus_node, 0);
		main_node.change_child(zero_node, 1);
	}
	return;
}

void Change_sign(Node& current_node, Node& parent_node, int position)
{
	if (current_node.get_value() == "-")
	{
		parent_node.insert_children(position, current_node.get_children());
		parent_node.erase_child(position + 1);
	}
	else
	{
		Node* minus_node = new Node;
		minus_node->set_value("-");
		minus_node->set_type(operation);
		minus_node->add_child(&current_node);

		parent_node.insert_children(position, *minus_node);
		parent_node.erase_child(position + 1);
	}
	return;
}

void Uniting_pluses_minuses_multiplications(Node& current_node, Node& parent_node)
{
	for (int j = 0; j < current_node.get_children().size(); j++)
	{
		if (current_node.get_children()[j]->get_type() == operation)
		{
			Uniting_pluses_minuses_multiplications(*current_node.get_children()[j], current_node);
		}
	}

	for (int i = 0; i < current_node.get_children().size(); i++)
	{
		if (current_node.get_value() == "+" && current_node.get_children()[i]->get_value() == "+")
		{
			int size = current_node.get_children()[i]->get_children().size();
			current_node.insert_children(i, current_node.get_children()[i]->get_children());
			current_node.erase_child(i + size);
		}

		if (current_node.get_value() == "-" && current_node.get_children()[i]->get_value() == "+")
		{
			current_node.set_value("+");
			current_node.set_children(current_node.get_children()[i]->get_children());

			for (int i = 0; i < current_node.get_children().size(); i++)
			{
				Node* minus_node = new Node;
				minus_node->set_value("-");
				minus_node->set_type(operation);
				minus_node->add_child(current_node.get_children()[i]);

				current_node.insert_children(i, *minus_node);
				current_node.erase_child(i + 1);

				Uniting_pluses_minuses_multiplications(*current_node.get_children()[i], current_node);
			}
		}

		if (current_node.get_value() == "-" && current_node.get_children()[0]->get_value() == "-")
		{
            parent_node.insert_children(i, current_node.get_children()[0]->get_children());
			parent_node.erase_child(i + 1);
		}

		if (current_node.get_value() == "*" && current_node.get_children()[i]->get_value() == "*")
		{
			int size = current_node.get_children()[i]->get_children().size();
			current_node.insert_children(i, current_node.get_children()[i]->get_children());
			current_node.erase_child(i + size);
		}
	}

	return;
}

void Sorting_in_alphabet_order(Node& current_node)
{
	if (current_node.get_type() == operand)
	{
		return;
	}
	else
	{
		if (current_node.get_type() == operation)
		{
			for (int i = 0; i < current_node.get_children().size(); i++)
			{
				Sorting_in_alphabet_order(*current_node.get_children()[i]);
			}
			Sorting_multiplication_and_sum_operands(current_node);
		}
		return;
	}
	return;
}

void Sorting_multiplication_and_sum_operands(Node& current_node)
{
	Node* temp_node;
	if (current_node.get_value() == "*" || current_node.get_value() == "+")
	{
		for (int i = 0; i < current_node.get_children().size(); i++)
		{
			for (int j = 0; j < current_node.get_children().size(); j++)
			{

				if (current_node.get_children()[i]->get_type() == operand && current_node.get_children()[j]->get_type() == operand
					&& i<j
					&& current_node.get_children()[i]->get_value() > current_node.get_children()[j]->get_value())
				{
					temp_node = current_node.get_children()[i];
					current_node.change_child(current_node.get_children()[j], i);
					current_node.change_child(temp_node, j);
				}

				if (current_node.get_children()[i]->get_type() == operation && current_node.get_children()[j]->get_type() == operation
					&& i < j)
				{
					QString operand_value1 = "";
					QString operand_value2 = "";
					Search_for_first_operand(*current_node.get_children()[i], &operand_value1);
					Search_for_first_operand(*current_node.get_children()[j], &operand_value2);

					if (operand_value1 > operand_value2)
					{
						temp_node = current_node.get_children()[i];
						current_node.change_child(current_node.get_children()[j], i);
						current_node.change_child(temp_node, j);
					}
				}

				if (current_node.get_children()[i]->get_type() == operation && current_node.get_children()[j]->get_type() == operand
					&& i < j)
				{
					QString operand_value = "";
					Search_for_first_operand(*current_node.get_children()[i], &operand_value);
					if (operand_value > current_node.get_children()[j]->get_value())
					{
						temp_node = current_node.get_children()[i];
						current_node.change_child(current_node.get_children()[j], i);
						current_node.change_child(temp_node, j);
					}
				}

				if (current_node.get_children()[i]->get_type() == operand && current_node.get_children()[j]->get_type() == operation
					&& i < j)
				{
					QString operand_value = "";
					Search_for_first_operand(*current_node.get_children()[j], &operand_value);
					if (current_node.get_children()[i]->get_value() > operand_value)
					{
						temp_node = current_node.get_children()[i];
						current_node.change_child(current_node.get_children()[j], i);
						current_node.change_child(temp_node, j);
					}
				}
			}
		}
	}
	return;
}

void Search_for_first_operand(Node& current_node, QString* operand_value)
{
	for (int i = 0; i < current_node.get_children().size(); i++)
	{
		if (current_node.get_children()[i]->get_type() == operation)
		{
			Search_for_first_operand(*current_node.get_children()[i], operand_value);
		}
		else
		{
			if (*operand_value == "")
				*operand_value = current_node.get_children()[i]->get_value();
		}
	}
	return;
}

void Write_nodes(QTextStream& out, Node& current_node)
{
	if (current_node.get_type() == operand)
	{
		out << "<operand>" << current_node.get_value() << "</operand>" << endl;
		return;
	}
	else
	{
		if (current_node.get_type() == operation)
		{
			QString correct_value = current_node.get_value();
			if (current_node.get_value() == ">")
				correct_value = "&gt;";
			else
			{
				if (current_node.get_value() == "<")
					correct_value = "&lt;";
				else
				{
					if (current_node.get_value() == "<=")
						correct_value = "&lt;=";
					else
					{
						if (current_node.get_value() == ">=")
							correct_value = "&gt;=";
					}
				}
			}

			out << "<operation symbol=\"" << correct_value << "\">" << endl;

			for (int i = 0; i < current_node.get_children().size(); i++)
			{
				Write_nodes(out, *current_node.get_children()[i]);
			}
			out << "</operation>" << endl;
		}
		return;
	}
}
