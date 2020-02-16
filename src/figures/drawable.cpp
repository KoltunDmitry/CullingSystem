#include "drawable.hpp"

Drawable::Drawable()
{
	static int counterID = 0;
	m_id = counterID++;
}
