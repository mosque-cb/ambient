(setq material
      '(
        1350112
        5571644
        2451989
        9870846
        15587475
        ))

(defun sum(lst)
  (if (eq lst nil)
      0
    (add (car lst) 
         (sum (cdr lst)))))

(defun abs(lst all material_str)
  (if (eq lst nil)
      nil
    (progn
      (print (concat (itoa (car lst)) 
                     (storage /) 
                     'CURVEL
                     (addconcat material_str)
                     'CURVER
                     (storage =)
                     (div (mul 100 (car lst))
                          all)
                     (storage %)))
      (abs (cdr lst) 
           all
           material_str))))

(abs material 
     (sum material)
     material)
