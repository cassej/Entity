Entity
======

Simple, but very useful extension to keep your entity object data changes

Instalation
===========

Download and install the extension:

```sh
git clone https://github.com/cassej/Entity.git
cd Entity
phpize
./configure
make
sudo make install
```

Open the php.ini file for editing and add the following line at the end of the file:

```
extension = entity.so
```
To enable extension:

#### Don`t forget to restart PHP-FPM!

How to use
==========

Create new class which will extend \Entity, for example:

```php
class User extends \Entity
{
  public $id = 0;
  public $email = '';
  public $password = '';
  public $permissions = 0;
}
```

Now objects of your class will have only that class variables you defined in class and new two methods:

```php
$User = new User();
$User->id = 1;
$User->email = 'test@example.com';
$User->password = 'secret';
$User->notClassVariable = 'test'; // This variable will not be created, 
                                  // only variables that were defined 
                                  // in class can be used 

var_dump($User->getData());     // This method will return all 
                                // class variables as array

var_dump($User->getChanges());  // This method will return an 
                                //array of changed class variables
```

Contructor of this class accepts an associative array which values will translated to class variables, for example:

```php
$User = new User(
  array(
    'id' => 1,
    'email' => 'test@example.com',
    'password' => 'secret'
  )
)

var_dump($User->getChanges());  // return an empty array
$User->email = 'another@example.com';
var_dump($User->getChanges()); // return array('email' => 'another@example.com')
```
