# Grammars

---

## Orignal

1. `program → declaration-list | comment | include_command`
2. `declaration-list → declaration-list declaration | declaration`
3. `declaration → var-declaration | fun-declaration`
4. `var-declaration → type-specifier ID ;`
5. `type-specifier → Imw | SIMw | Chj | Series | IMwf | SIMwf | NOReturn`
6. `fun-declaration → type-specifier ID ( params ) compound-stmt | comment type-specifier ID`
7. `params → param-list | NOReturn | ε`
8. `param-list → param-list , param | param`
9. `param → type-specifier ID`
10. `compound-stmt → { comment local-declarations statement-list } | { local-`declarations statement-list }
11. `local-declarations → var-declaration local-declarations| ε` ==_no need to restructure this_==

12. `statement-list → statement statement-list | ε`
13. `statement → expression-stmt | compound-stmt | selection-stmt | iteration-stmt | jump-stmt`
14. `expression-stmt → expression ; | ;`
15. `selection-stmt → IfTrue ( expression ) statement | IfTrue ( expression ) statement Otherwise statement`
16. `iteration-stmt → RepeatWhen ( expression ) statement | Reiterate ( expression ; expression ; expression ) statement`
17. `jump-stmt → Turnback expression ; | Stop ;`
18. `expression → id-assign = expression | simple-expression | id-assign`
19. `id-assign → ID`
20. `simple-expression → additive-expression relop additive-expression | additive-expression`
21. `relop → <= | < | > | >= | == | != | && | ||`
22. `additive-expression → additive-expr ession addop term | term`
23. `addop → + | -`
24. `term → term mulop factor | factor`
25. `mulop → * | /`
26. `factor → ( expression ) | id-assign | call | num`
27. `call → ID ( args )`
28. `args → arg-list | ε`
29. `arg-list → arg-list , expression | expression`
30. `num → Signed num | Unsigned num`
31. `Unsigned num → value`
32. `Signed num → pos-num | neg-num`
33. `pos-num → + value`
34. `neg-num → - value`
35. `value → INT_NUM | FLOAT_NUM`
36. `comment → /@ STR @/ | /^ STR`
37. `include_command → include ( F_name.txt );`
38. `F_name → STR`

---

## Right-Recursive Grammar

---

1. `program → declaration-list | comment | include_command`
2. `declaration-list → declaration d_list`
3. `d_list -> declaration d_list | e`
4. `declaration → var-declaration | fun-declaration`

5. `var-declaration → type-specifier ID ;`
6. `type-specifier → Imw | SIMw | Chj | Series | IMwf | SIMwf | NOReturn`

7. `fun-declaration → type-specifier ID ( params ) compound-stmt | comment type-specifier ID`
8. `params → param-list | NOReturn | ε`
9. `param-list → param p_list`
10. `p_list -> , param p_list | e`
11. `param → type-specifier ID`

12. `compound-stmt → { comment local-declarations statement-list } | { local-declarations statement-list }`
13. `local-declarations → local-declarations var-declaration | ε`
14. `statement-list → statement-list statement | ε`
15. `statement → expression-stmt | compound-stmt | selection-stmt | iteration-stmt | jump-stmt`
16. `expression-stmt → expression ; | ;`
17. `selection-stmt → IfTrue ( expression ) statement | IfTrue ( expression ) statement Otherwise statement`
18. `iteration-stmt → RepeatWhen ( expression ) statement | Reiterate ( expression ; expression ; expression ) statement`
19. `jump-stmt → Turnback expression ; | Stop ;`
20. `expression → id-assign = expression | simple-expression | id-assign`
21. `id-assign → ID`
22. `simple-expression → additive-expression relop additive-expression | additive-expression`
23. `relop → <= | < | > | >= | == | != | && | ||`
24. `additive-expression → additive-expression addop term | term`
25. `addop → + | -`
26. `term → term mulop factor | factor`
27. `mulop → * | /`
28. `factor → ( expression ) | id-assign | call | num`
29. `call → ID ( args )`

30. `args → arg-list | ε`
31. `arg-list → expression a_list
32. `a_list -> , expression a_list | e`

33. `num → Signed num | Unsigned num`
34. `Unsigned num → value`
35. `Signed num → pos-num | neg-num`
36. `pos-num → + value`
37. `neg-num → - value`
38. `value → INT_NUM | FLOAT_NUM`
39. `comment → /@ STR @/ | /^ STR`
40. `include_command → include ( F_name.txt );`
41. `F_name → STR`
