/**
 * @file
 * @brief Заголовочный файл для класса узла B-дерева
 */
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <memory>
#include <span>

#include "BTreeKey.hpp"

/**
 * @brief Пространство имен для всего связанного с B-деревьями
 */
namespace btree {

/**
 * @brief Узел B-дерева
 *
 * @tparam ValueT Тип хранимых значений в узле
 */
template <BTreeKey ValueT, std::size_t order> class BtreeNode final {
public:
  using value_type = ValueT; /// < Тип значений в узлах
  using value_reference_type = ValueT &;
  using value_const_reference_type = const ValueT &;
  using value_array_type = std::array<value_type, order - 1>;

  using child_type = BtreeNode<value_type, order>; /// < Тип дочерних узлов
  using child_ptr_type =
      std::unique_ptr<child_type>; /// < Тип указателя на дочерний узел
  using child_ptr_array_type = std::array<child_ptr_type, order>;
  using child_reference_type = child_type &;
  using child_const_reference_type = const child_type &;

  BtreeNode() = default; /// < Конструктор по умолчанию
  BtreeNode(const BtreeNode &) = default; /// < Конструктор копирования
  BtreeNode(BtreeNode &&) noexcept = default; /// < Конструктор перемещения
  ~BtreeNode() = default;                     /// < Деструктор

  BtreeNode &operator=(const BtreeNode &) = default;
  BtreeNode &operator=(BtreeNode &&) noexcept = default;

  /**
   * @brief Получить значение по индексу (безопасно)
   * При недопустимом индексе выбрасывается исключение
   * @param index Индекс получаемого элемента
   * @return value_const_reference_type Константная ссылка на элемент
   */
  value_const_reference_type at(const std::size_t index) const {
    return m_values.at(index);
  }
  /**
   * @brief Получить значение по индексу (опасно)
   * Неопределенное поведение при выходе за границы
   * @param index Индекс получаемого элемента
   * @return value_const_reference_type Константная ссылка на элемент
   */
  value_const_reference_type operator[](const std::size_t index) const {
    return m_values[index];
  }

  child_const_reference_type child_at_left(const std::size_t index) const {
    return *(m_childen[index]);
  }
  child_reference_type child_at_left(const std::size_t index) {
    return *(m_childen[index]);
  }

  child_const_reference_type child_at_right(const std::size_t index) const {
    return *(m_childen[index + 1]);
  }
  child_reference_type child_at_right(const std::size_t index) {
    return *(m_childen[index + 1]);
  }

  /**
   * @brief Проверяет заполнен ли узел
   * @return true Узел заполнен (вставка невозможна без разделения)
   * @return false Узел не заполнен (вставка возможна без разделения)
   */
  inline bool is_full() { return m_value_count >= order; }

  /**
   * @brief Освобождает место для вставки элемета
   * @details Освобождает место под вставку элемента сдвигая значения в узле
   * Не удаляет исходный элемент с места вставки
   * @param index Индекс места для вставки
   */
  inline void free_place(const std::size_t index) {
    for (auto i = m_value_count; i > index; --i) {
      m_values[i] = m_values[i - 1];
    }
  }

  /**
   * @brief Находит место для вставки элемента с указанным значением
   *
   * @param new_val Значение нового элемента
   * @return std::size_t Индекс места, в которое нужно вставить новый элемент
   */
  inline std::size_t find_insertion_place(value_reference_type new_val) const {
    const auto it = std::upper_bound(m_values.first(),
                                     m_values.first() + m_value_count, new_val);
    return std::distance(m_values.first(), it);
  }

  inline std::size_t value_count() const { return m_value_count; }

private:
  value_array_type m_values; /// < Массив значений
  child_ptr_array_type m_childen; /// < Массив указателей на дочерние узлы

  std::size_t m_value_count;
  std::size_t m_childen_count;

  bool m_is_leaf;
};

}; // namespace btree