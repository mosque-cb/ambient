(defun  cadr  (n)
  (car  (cdr  n)))

(defun  cdar  (n)
  (cdr  (car  n)))

(defun printlst (n)
  (if  (eq   n  nil)
      nil
    (progn
      (print  (car  n) )
      (printlst (cdr n) ))))

(defun  wrap_defun (lst )
  (progn
    (setq  global_defun  (cons  lst global_defun))
    (print  'wrap_defun)))

(defun  funp(name global)
  (if  (eq  global  nil)
      0
    (if  (eq  name  (car (car global)))
        1
      (funp  name  (cdr  global)))))

(defun  find_expr(name global)
  (if  (eq  global  nil)
      nil
    (if  (eq  name  (car (car global)))
        (cadr (cdar global))
      (find_expr  name  (cdr  global)))))

(defun  find_arg(name global)
  (if  (eq  global  nil)
      nil
    (if  (eq  name  (car (car global)))
        (cadr (car global))
      (find_arg  name  (cdr  global)))))

(defun  bindvars  (arg value env)
  (cons
   (bindvars_help arg value)
   env))

(defun  bindvars_help  (arg value )
  (if  (eq  arg  nil)
      nil
    (cons  ( cons  (car arg)  (cons (car value) nil))
           (bindvars_help  (cdr  arg) (cdr value)))))

(defun  var_find (arg env)
  (if  (eq  env  nil)
      0
    (if  (eq  arg  (car  (car env)))
        1
      (var_find arg  (cdr env)))))

(defun  var_fetch (arg env)
  (if  (eq  env  nil)
      nil
    (if  (eq  arg  (car  (car env)))
        (cadr  (car env))
      (var_fetch arg  (cdr env)))))

(defun  findvar  (arg  env)
  (if  (eq  env  nil)
      nil
    (if  (var_find   arg    (car env))
        (var_fetch   arg    (car env))
      (findvar  arg  (cdr env)))))

(defun   eval_para (lst env cont)
  (if  (eq lst nil)
      (funcall  cont nil)
    (eval_para  (cdr lst)
                env  
                (lambda (y)
                  (wrap_eval  (car lst)  
                              env   
                              (lambda  (x) 
                                (progn
                                  (funcall  cont 
                                            (cons  x y)))))))))

(defun   wrap_eval (lst env cont)
  (if  (eq  lst nil)
      (funcall  cont lst)
    (if  (digitp  lst )
        (funcall cont lst)
      (if  (charp  lst)
          (progn
            (funcall  cont   (findvar lst 
                                      env)))
        (if  (eq (car lst)  'mod)
            (eval_para  (cdr lst) 
                        env  
                        (lambda  (lst ) 
                          (funcall cont  
                                   (mod  (car lst) 
                                         (cadr lst)))))
          (if  (eq (car lst)  'add)
              (progn
                (eval_para  (cdr lst) 
                            env  
                            (lambda  (lst )
                              (funcall  cont  
                                        (progn 
                                          (add  (car lst) 
                                                (cadr lst)))))))
            (if  (eq (car lst)  'minus)
                (eval_para  (cdr lst) 
                            env  
                            (lambda  (lst )
                              (funcall  cont 
                                        (minus  (car lst) 
                                                (cadr lst)))))
              (if  (eq (car lst)  'random)
                  (funcall cont		  
                           (random  (cadr lst ) ))
                (if  (eq (car lst)  'cons)
                    (eval_para  (cdr lst) 
                                env 
                                (lambda  (lst ) 
                                  (progn
                                    (funcall  cont  
                                              (cons  (car lst) 
                                                     (cadr lst))))))
                  (if  (eq (car lst)  'cdr)
                      (eval_para  (cdr lst) 
                                  env 
                                  (lambda  (lst ) 
                                    (funcall  cont  
                                              (cdr  (car lst)))))
                    (if  (eq (car lst)  'car)
                        (eval_para  (cdr lst) 
                                    env 
                                    (lambda  (lst ) 
                                      (funcall  cont  
                                                (car  (car lst)))))
                      (if  (eq (car lst)  'printlst)
                          (eval_para  (cdr lst) 
                                      env 
                                      (lambda  (lst ) 
                                        (printlst  (car lst ))
                                        (funcall  cont (car lst))))
                        (if  (eq (car lst)  'if)
                            (wrap_eval  (car (cdr lst))  
                                        env 
                                        (lambda  ( x)  
                                          (if   x
                                              (wrap_eval  (cadr (cdr lst)) 
                                                          env 
                                                          cont)
                                            (wrap_eval  
                                             (cadr 
                                              (cdr (cdr lst))) 
                                             env 
                                             cont))))
                          (if  (eq (car lst)  'eq)
                              (eval_para  (cdr lst) 
                                          env  
                                          (lambda  (lst )
                                            (if (eq  (car lst) 
                                                     (cadr lst))
                                                (funcall  cont  1)
                                              (funcall  cont  0))))
                            (if  (eq (car lst)  'defun)
                                (funcall  cont		
                                          (wrap_defun  (cdr lst ) ))
                              (if  (funp  (car lst) global_defun)
                                  (eval_para 
                                   (cdr lst) 
                                   env
                                   (lambda ( parame )
                                     (wrap_eval  (find_expr  (car lst) 
                                                             global_defun)
                                                 (bindvars   (find_arg (car lst) 
                                                                       global_defun) 
                                                             parame
                                                             env  )
                                                 (lambda (result ) 
                                                   (progn
                                                     (funcall cont 
                                                              result))))))
                                nil))))))))))))))))

(defun  generand  (count range)
  (if  (eq  count  0)
      nil
    (cons    (random  range)
             (generand  (minus  count  1)
                        range))))

(defun once  ( )
  (progn 
    (display )
    (setq  global_defun  nil)
    (main basic_defun )
    (main basic_apply )
    (setq  global_defun  nil)
    (display)
    (print  'hell)))

(defun  autotest(num)
  (progn
    (once)
    (print (concat (storage times is ) num))
    (autotest (add num 1))))

(defun  main (lst  )
  (if  (eq  lst  nil)
      nil
    (progn
      (wrap_eval  (car lst) nil (lambda (x)  (print x))  )
      (main (cdr lst)  ))))

(define  basic_defun   '(  (defun  sum (a b)
                             (add  a  b))

                           (defun  reactor (from  to )
                             (if  (eq  0  to)
                                 nil
                               (cons  from  
                                      (reactor  (add  from  1)
                                                (minus to 1)))))

                           ))

(define  basic_apply    '( 
                          (printlst  (reactor  (random 1000)
                                               (sum 1 (random 10))))
                          ))


(define  basic_show    'hello)
(print   'init_object)
(once)
(autotest 0)

