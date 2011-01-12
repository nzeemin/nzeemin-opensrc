using System;

namespace CoopCafe
{
    class Program
    {
        private static decimal p3 = 5.0m;  // Рыночная стоимость продуктов на одного клиента без учета скидки
        private static decimal p4 = 0.0m;  // Фактическая сумма, затраченная в среднем одним посетителем
        private static decimal p5 = 7.0m;  // Деньги которые клиент рассчитывал оставить
        private static decimal p6 = 0.0m;  // Сколько клиентов мы готовы обслужить
        private static decimal p8 = 0.03m;  // Отчисления с оборота на собственные нужды
        private static decimal pa = 0.0m;  // Нереализованные остатки продукции
        private static decimal pb = 0.0m;  // Количество необслуженных либо количество пустых мест
        private static decimal pc = 30.0m;  // Постоянные клиенты
        private static decimal pd = 250.0m;  // Деньги
        
        private static decimal p2, p9;  // Оперативные регистры

        private static decimal p6prev, pdprev;

        static void Main(string[] args)
        {
            Console.Out.WriteLine("Кооперативное кафе");
            Console.Out.WriteLine("  Автор идеи: Леонид Самутин");
            Console.Out.WriteLine("  Автор программы для ПМК: Вячеслав Алексеев (ТМ 05'1988)");
            Console.Out.WriteLine("  Автор конверсии на C#: Никита Зимин, 2011");
            Console.Out.WriteLine();

            Console.ForegroundColor = ConsoleColor.White;
            Console.Out.WriteLine("Цена продуктов на 1-го клиента:\t{0:0.00}", p3);
            Console.Out.WriteLine("Клиент рассчитывает потратить:\t{0:0.00}", p5);
            Console.Out.WriteLine("Постоянных клиентов:\t\t{0}", (int)pc);
            Console.Out.WriteLine("Деньги:\t\t\t\t{0:0.00}", pd);

            for (; ; )
            {
                //Console.ResetColor();
                //Console.Out.Write("Продолжаем (Enter) / Выходим (Q)? ");
                //string str = Console.In.ReadLine();
                //if (str == "q" || str == "Q")
                //    break;

                p6prev = p6;
                pdprev = pd;

                Console.Out.WriteLine();
                InputExpenses("Затраты на продукты?\t\t", true);

                decimal skidka = (decimal)Math.Round(Math.Sqrt((double)p2), 2);
                if (skidka >= p2) skidka = 0;
                pd += skidka;
                p9 = p2;  // Запоминаем сколько потрачено на продукты

                pa = p9 / p3 + pa;  // Вычисляем количество порций

                Console.Out.WriteLine("Скидка:\t\t\t\t{0:0.00}", skidka);
                Console.Out.WriteLine("Деньги:\t\t\t\t{0:0.00}", pd);
                InputExpenses("Затраты на рекламу?\t\t", false);

                // Учитываем эффект рекламы и постоянных клиентов
                pc = (decimal)(Math.Atan(Math.Sqrt((double)p2)) * (double)pc / 2.0 + (double)pc);

                Console.Out.WriteLine("Деньги:\t\t\t\t{0:0.00}", pd);
                InputExpenses("Накладные расходы?\t\t", true);

                Console.Out.WriteLine("Деньги:\t\t\t\t{0:0.00}", pd);
                Console.Out.WriteLine();

                p2 = p2 * 2;  // Наши возможности по переработке продукции
                // Сопоставляем возможности по переработке с реальным количеством продуктов
                decimal temp = pa - p2;
                if (temp < 0)
                {
                    p6 = pa;
                    pa = 0;
                }
                else
                {
                    pa = (decimal)((double)temp / Math.PI);
                    p6 = p2;
                }
                Console.Out.WriteLine("Готовы обслужить клиентов:\t{0}", (int)p6);

                // Определяем общее количество пришедших в кафе
                p4 = 0;
                if (p6 > 0.0m)
                    p4 = (p2 + p9) / p6;

                decimal temp2 = pc - pb * 2;
                if (temp2 < 0)
                {
                    temp2 = 0;
                }
                decimal temp3 = temp2 * p5 / p4;
                Console.Out.WriteLine("Количество пришедших клиентов:\t{0}", (int)temp3);

                // Определяем сколько неудовлетворенных клиентов или свободных мест
                pb = temp3 - p6;
                if (pb < 0)
                {
                    p6 = temp3;
                    pa = -pb / 3;
                }

                // Подсчет прибыли
                pc = p6;
                decimal temp4 = pc * p4 + pd;
                pd = Math.Round(temp4 - temp4 * p8, 2);

                Console.Out.WriteLine("Обслужили клиентов:\t\t{0}\t({1:+0;-0})", (int)p6, (int)p6 - (int)p6prev);
                if (pb > 0)
                    Console.Out.WriteLine("Необслуженных клиентов:\t\t{0:0.}", pb);
                else
                    Console.Out.WriteLine("Невостребованных порций:\t{0:0.}", -pb);
                Console.Out.WriteLine("Посетитель потратил, в среднем:\t{0:0.00}", p4);
                Console.Out.WriteLine("Осталось порций:\t\t{0:0.0}", pa);
                Console.Out.WriteLine("Постоянных клиентов:\t\t{0}", (int)pc);
                Console.Out.WriteLine("Деньги:\t\t\t\t{0:0.00}\t({1:+0.00;-0.00})", pd, pd - pdprev);
            }

            Console.ResetColor();
        }

        static void InputExpenses(string prompt, bool nonZero)
        {
            for (; ; )
            {
                Console.ForegroundColor = ConsoleColor.Cyan;
                Console.Out.Write(prompt);
                Console.ForegroundColor = ConsoleColor.Yellow;
                string str = Console.In.ReadLine();
                Console.ForegroundColor = ConsoleColor.White;

                if (str == "q" || str == "Q")
                {
                    Console.ResetColor();
                    Environment.Exit(0);
                }

                if (!decimal.TryParse(str, out p2))
                    continue;
                if (p2 < 0 || pd - p2 < 0)
                    continue;
                p2 = Math.Round(p2, 2);
                if (p2 == 0 && nonZero)
                    continue;
                break;
            }
            pd -= p2;
        }
    }
}
