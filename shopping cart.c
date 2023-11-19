#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define ADMIN_PASSWORD "adminpass"

// Structure for user
typedef struct User
{
    char fullName[100];
    char username[50];
    char password[50];
    char userType[20];
    struct User* next;
} User;

// Structure for product data
typedef struct Product
{
    char name[50];
    double price;
    int quantity;
    int soldQuantity;
    struct Product* next;
} Product;

// Structure for the shopping cart item
typedef struct CartItem
{
    Product* product;
    char productName[50];
    double price;
    int quantity;
    struct CartItem* next;
} CartItem;

// Function declarations
void displayMainMenu();
int verifyAdminPassword();
User* loadUserData();
Product* loadProductData();
void saveUserToFile(User* userList);
void saveProductToFile(Product* productList);
Product* findProduct(Product* productList, const char* productName);
void freeCart(CartItem* shoppingCart);
void adminPanel(User** userList, Product** productList);
void displayUserList(User* userList);
User* addUserToList(User* userList, const char* fullName, const char* username, const char* password, const char* userType);
User* removeUserFromList(User* userList, const char* usernameToDelete);
void displayProductList(Product* productList);
void displaySoldProducts(Product* productList);
Product* addProductToList(Product* productList, const char* productName, double price, int quantity);
Product* removeProductFromList(Product* productList, const char* productNameToDelete);
User* findUser(User* userList, const char* username);
User* userSignUp(User** userList);
User* userLogin(User* userList);
void viewUserProfile(User* user);
void updateUserProfile(User* user);
void viewProducts(Product* productList);
void searchProduct(Product* productList);
void addToCart(User* user, Product* productList, CartItem** shoppingCart);
void viewShoppingCart(User* user, CartItem** shoppingCart, Product* productList);
void removeFromCart(CartItem** shoppingCart);
void checkout(User* user, CartItem** shoppingCart, Product* productList);
void showInvoice(CartItem* cart, User* user, double totalCost, const char* paymentMethod);
void leaveReview(Product* product, User* user);
void userPanel(User** userList, Product** productList, CartItem** shoppingCart);

// display the main menu
void displayMainMenu()
{
    printf("\n                                    ----------------------------\n");
    printf("                                           Welcome To Our       \n");
    printf("                                        Shopping Cart System    \n");
    printf("                                    ----------------------------\n\n");
    printf("                                    --- Main Menu: ---\n");
    printf("                                    1. Admin Panel\n");
    printf("                                    2. User Panel\n");
    printf("                                    3. Exit\n");
}

// verify the admin password
int verifyAdminPassword()
{
    char inputPassword[50];
    printf("                                    Enter the admin password: ");
    //scanf("%s", inputPassword);
    int i = 0;
    char ch;
    while (1) {
        ch = _getch();
        if (ch == '\r') {
            break;
        } else if (ch == 8) { // Backspace
            if (i > 0) {
                i--;
                inputPassword[i] = '\0';
                printf("\b \b");
            }
        } else {
            inputPassword[i] = ch;
            i++;
            printf("*");
        }
    }
    inputPassword[i] = '\0';
    printf("\n");

    if (strcmp(inputPassword, ADMIN_PASSWORD) == 0)
    {
        return 1; // Password is correct
    }
    else
    {
        return 0; // Password is incorrect
    }
}

User* createUser(const char* fullName, const char* username, const char* password, const char* userType) {
    User* newUser = (User*)malloc(sizeof(User));
    if (newUser == NULL) {
        fprintf(stderr, "                                    Memory allocation failed for user creation.\n");
        exit(1);
    }

    strcpy(newUser->fullName, fullName);
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    strcpy(newUser->userType, userType);

    newUser->next = NULL;

    return newUser;
}

Product* createProduct(const char* name, double price, int quantity) {
    Product* newProduct = (Product*)malloc(sizeof(Product));
    if (newProduct == NULL) {
        fprintf(stderr, "                                    Memory allocation failed for product creation.\n");
        exit(1);
    }

    strcpy(newProduct->name, name);
    newProduct->price = price;
    newProduct->quantity = quantity;

    newProduct->next = NULL;

    return newProduct;
}

// Define the function to save users to a file
void saveUserToFile(User* userList) {
    FILE* file = fopen("users.txt", "w");
    if (file == NULL) {
        printf("Error: Unable to open the users file for writing.\n");
        return;
    }

    User* currentUser = userList;
    while (currentUser != NULL) {
        fprintf(file, "\nFullName: %s\nUsername: %s\nPassword: %s\nUserType: %s\n",
                currentUser->fullName, currentUser->username, currentUser->password, currentUser->userType);
        currentUser = currentUser->next;
    }

    fclose(file);
}

// Define the function to load users from a file
User* loadUserData() {
    User* userList = NULL;
    FILE* file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("Warning: No user data found.\n");
        return NULL;
    }

    while (1) {
        char username[50];
        char password[50];
        char fullName[100];
        char userType[20];

        if (fscanf(file, "\nFullName: %99[^\n]\nUsername: %49s\nPassword: %49s\nUserType: %19s\n",
                   fullName, username, password, userType) == 4) {
            User* newUser = createUser(fullName, username, password, userType);
            newUser->next = userList;
            userList = newUser;
        } else {
            break;
        }
    }

    fclose(file);
    return userList;
}

// Define the function to save products to a file
void saveProductToFile(Product* productList) {
    FILE* file = fopen("products.txt", "w");
    if (file == NULL) {
        printf("Error: Unable to open the products file for writing.\n");
        return;
    }

    Product* currentProduct = productList;
    while (currentProduct != NULL) {
        fprintf(file, "\nName: %s\nPrice: %.2lf\nQuantity: %d\n",
                currentProduct->name, currentProduct->price, currentProduct->quantity);
        currentProduct = currentProduct->next;
    }

    fclose(file);
}

// Define the function to load products from a file
Product* loadProductData() {
    Product* productList = NULL;
    FILE* file = fopen("products.txt", "r");
    if (file == NULL) {
        printf("Warning: No product data found.\n");
        return NULL;
    }

    while (1) {
        char name[100];
        double price;
        int quantity;
        int soldQuantity;

        if (fscanf(file, "\nName: %99s\nPrice: %lf\nQuantity: %d\n",
                   name, &price, &quantity) == 3) {
            Product* newProduct = createProduct(name, price, quantity);
            newProduct->next = productList;
            productList = newProduct;
        } else {
            break;
        }
    }

    fclose(file);
    return productList;
}

// Define the findProduct function (assuming it's a separate function)
Product* findProduct(Product* productList, const char* productName) {
    Product* currentProduct = productList;

    while (currentProduct != NULL) {
        if (strcmp(currentProduct->name, productName) == 0) {
            return currentProduct;
        }
        currentProduct = currentProduct->next;
    }

    return NULL; // Product not found
}

// free shopping cart
void freeCart(CartItem* shoppingCart)
{
    while (shoppingCart != NULL)
    {
        CartItem* temp = shoppingCart;
        shoppingCart = shoppingCart->next;
        free(temp);
    }
}

// Admin panel
void adminPanel(User** userList, Product** productList)
{
    int adminChoice;

    while (1)
    {
        printf("\n                                    --- Welcome To Admin Menu: ---\n");
        printf("                                    Admin Panel Menu:\n");
        printf("                                    1. Add User\n");
        printf("                                    2. Remove User\n");
        printf("                                    3. Display User\n");
        printf("                                    4. Add Product\n");
        printf("                                    5. Remove Product\n");
        printf("                                    6. Display Product\n");
        printf("                                    7. View Sold Products\n");
        printf("                                    8. Logout\n");
        printf("                                    Enter your choice: ");
        scanf("%d", &adminChoice);

        switch (adminChoice)
        {
        case 1: // Add User
            userSignUp(userList);
            break;

        case 2: // Remove User
        {
            char usernameToDelete[50];
            printf("\n                                    Enter the username to remove: ");
            scanf("%s", usernameToDelete);
            *userList = removeUserFromList(*userList, usernameToDelete);
        }
        break;

        case 3: // Display User
            displayUserList(*userList);
            break;

        case 4: // Add Product
        {
            char newProductName[50];
            double newProductPrice;
            int newProductQuantity;

            printf("\n                                    Enter new product name: ");
            scanf("%s", newProductName);

            printf("                                    Enter product price: ");
            scanf("%lf", &newProductPrice);

            printf("                                    Enter product quantity: ");
            scanf("%d", &newProductQuantity);

            *productList = addProductToList(*productList, newProductName, newProductPrice, newProductQuantity);
        }
        break;

        case 5: // Remove Product
        {
            char productNameToDelete[50];
            printf("\n                                    Enter the product name to remove: ");
            scanf("%s", productNameToDelete);
            *productList = removeProductFromList(*productList, productNameToDelete);
        }
        break;

        case 6: // Display Product
            displayProductList(*productList);
            break;

        case 7: // View Sold Products
            displaySoldProducts(*productList);
            break;

        case 8: // Logout
            printf("                                    Logged out of the admin panel.\n");
            return;

        default:
            printf("                                    Invalid choice. Please try again.\n");
            break;
        }
    }
}

// user sign-up
User* userSignUp(User** userList)
{
    char fullName[100], username[50], password[50], userType[20];

    printf("\n                                    Enter full name: ");
    scanf(" %[^\n]s", fullName);

    printf("                                    Enter new username: ");
    scanf("%s", username);

    printf("                                    Enter new password: ");
    scanf("%s", password);

    printf("                                    Enter user type: ");
    scanf("%s", userType);

    // Add the user to the user list
    *userList = addUserToList(*userList, fullName, username, password, userType);

    // file
    /*FILE* userFile;
    userFile = fopen("users.txt", "a");

    if (userFile == NULL)
    {
        printf("Error!\n");
        return NULL;
    }

    fprintf(userFile, "Full Name: %s\n", fullName);
    fprintf(userFile, "Username: %s\n", username);
    fprintf(userFile, "Password: %s\n", password);
    fprintf(userFile, "User Type: %s\n", userType);
    fprintf(userFile, "--------------------------------------\n");

    // Close the file
    fclose(userFile);
    */

    // Save the user list to the file
    saveUserToFile(*userList);

    return *userList; // Return the updated user list
}

// add a user to list
User* addUserToList(User* userList, const char* fullName, const char* username, const char* password, const char* userType)
{
    User* newUser = (User*)malloc(sizeof(User));

    if (newUser == NULL)
    {
        printf("                                    Unable to add a new user.\n");
        return userList;
    }

    // Initialize the user's data
    strncpy(newUser->fullName, fullName, sizeof(newUser->fullName) - 1);
    strncpy(newUser->username, username, sizeof(newUser->username) - 1);
    strncpy(newUser->password, password, sizeof(newUser->password) - 1);
    strncpy(newUser->userType, userType, sizeof(newUser->userType) - 1);
    newUser->next = NULL;

    if (userList == NULL)
    {
        // If the user list is empty, set the new user as the head
        userList = newUser;
    }
    else
    {
        // Find the end of the user list and add the new user
        User* current = userList;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = newUser;
    }

    printf("                                    User '%s' has been added to the system.\n", newUser->username);

    return userList;
}


// Function to remove a user
User* removeUserFromList(User* userList, const char* usernameToDelete)
{
    User* current = userList;
    User* previous = NULL;

    while (current != NULL)
    {
        if (strcmp(current->username, usernameToDelete) == 0)
        {
            if (previous == NULL)
            {
                userList = current->next;
            }
            else
            {
                previous->next = current->next;
            }
            free(current);
            printf("                                    User '%s' removed successfully.\n", usernameToDelete);
            return userList;
        }

        previous = current;
        current = current->next;
    }

    printf("                                    User '%s' not found. Cannot remove.\n", usernameToDelete);
    return userList;
}


// display user data from the user linked list
void displayUserList(User* userList)
{
    if (userList == NULL)
    {
        printf("                                    User list is empty.\n");
        return;
    }

    User* current = userList;
    while (current != NULL)
    {
        printf("\n                                    ----------------------\n");
        printf("                                    Full Name: %s\n", current->fullName);
        printf("                                    Username: %s\n", current->username);
        printf("                                    User Type: %s\n", current->userType);
        printf("                                    ----------------------\n");

        current = current->next;
    }
}


// add a new product to the product linked list
Product* addProductToList(Product* productList, const char* productName, double price, int quantity)
{
    Product* newProduct = (Product*)malloc(sizeof(Product));
    if (newProduct == NULL)
    {
        printf("                                    Unable to add new product.\n");
        return productList;
    }

    strcpy(newProduct->name, productName);
    newProduct->price = price;
    newProduct->quantity = quantity;
    newProduct->next = productList;

    printf("                                    Product '%s' added successfully.\n", productName);
    return newProduct;
}

// remove a product from the product linked list
Product* removeProductFromList(Product* productList, const char* productNameToDelete)
{
    Product* current = productList;
    Product* previous = NULL;

    while (current != NULL)
    {
        if (strcmp(current->name, productNameToDelete) == 0)
        {
            if (previous == NULL)
            {
                productList = current->next;
            }
            else
            {
                previous->next = current->next;
            }
            free(current);
            printf("                                    Product '%s' removed successfully.\n", productNameToDelete);
            return productList;
        }

        previous = current;
        current = current->next;
    }

    printf("                                    Product '%s' not found. Cannot remove.\n", productNameToDelete);
    return productList;
}


// display product data from the product linked list
void displayProductList(Product* productList)
{
    if (productList == NULL)
    {
        printf("                                    Product list is empty.\n");
        return;
    }

    Product* current = productList;
    printf("\n                                    Product Name\tPrice\tQuantity\n");
    while (current != NULL)
    {
        printf("                                    %s\t%.2lf\t%d\n", current->name, current->price, current->quantity);

        current = current->next;
    }
}

// view sold products
void displaySoldProducts(Product* productList)
{
    printf("\n                                    Sold Products:\n");
    printf("                                    ---------------------------------------------------\n");

    FILE* soldProductsFile = fopen("sold_products.txt", "r");
    if (soldProductsFile == NULL)
    {
        printf("                                    No sold products yet.\n");
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), soldProductsFile))
    {
        printf("                                    %s", line);
    }

    fclose(soldProductsFile);

    printf("                                    ---------------------------------------------------\n");
}

// user panel operations
void userPanel(User** userList, Product** productList, CartItem** shoppingCart)
{
    User* currentUser = NULL;
    int userChoice;

    while (1)
    {
        printf("\n                                    --- Welcome To User Menu: ---\n");
        printf("                                    User Panel Menu:\n");
        printf("                                    1. Sign Up\n");
        printf("                                    2. Log In\n");
        printf("                                    3. Exit\n");
        printf("                                    Enter your choice: ");
        scanf("%d", &userChoice);

        switch (userChoice)
        {
        case 1: // Sign Up
            userSignUp(userList);
            break;

        case 2: // Log In
            currentUser = userLogin(*userList);
            break;

        case 3: // Exit
            printf("                                    Exiting the user panel.\n");
            return;
        }

        if (currentUser)
        {
            // Handle user interactions based on the logged-in user
            int loggedInChoice;
            while (1)
            {
                printf("\n                                    Logged In To User Menu:\n");
                printf("                                    1. View Profile\n");
                printf("                                    2. Update Profile\n");
                printf("                                    3. View Products\n");
                printf("                                    4. Search Product\n");
                printf("                                    5. Add to Cart\n");
                printf("                                    6. View Shopping Cart\n");
                printf("                                    7. Log Out\n");
                printf("                                    Enter your choice: ");
                scanf("%d", &loggedInChoice);

                switch (loggedInChoice)
                {
                case 1: // View Profile
                    viewUserProfile(currentUser);
                    break;

                case 2: // Update Profile
                    updateUserProfile(currentUser);
                    break;

                case 3: // View Products
                    viewProducts(*productList);
                    break;

                case 4: // Search Product
                    searchProduct(*productList);
                    break;

                case 5: // Add to Cart
                    addToCart(currentUser, *productList, shoppingCart);
                    break;

                case 6: // View Shopping Cart
                    viewShoppingCart(currentUser, shoppingCart, *productList);
                    break;

                case 7: // Log Out
                    printf("                                    Logged out of the user panel.\n");
                    //currentUser = NULL;
                    return;
                    break;
                }
            }
        }
    }
}

// find user
User* findUser(User* userList, const char* username)
{
    User* current = userList;
    while (current != NULL)
    {
        if (strcmp(current->username, username) == 0)
        {
            return current; // User found
        }
        current = current->next;
    }
    return NULL; // User not found
}

// user login
User* userLogin(User* userList)
{
    char username[50];
    char password[50];

    printf("\n                                    Enter your username: ");
    scanf("%s", username);

    // Find the user in the user list
    User* currentUser = findUser(userList, username);

    if (currentUser == NULL)
    {
        printf("                                    User '%s' does not exist. Please sign up or enter a valid username.\n", username);
        return NULL;
    }

    printf("                                    Enter your password: ");
    //scanf("%s", password);
    int i = 0;
    char ch;
    while (1) {
        ch = _getch();
        if (ch == '\r') {
            break;
        } else if (ch == 8) { // Backspace
            if (i > 0) {
                i--;
                password[i] = '\0';
                printf("\b \b");
            }
        } else {
            password[i] = ch;
            i++;
            printf("*");
        }
    }
    password[i] = '\0';
    printf("\n");

    if (strcmp(currentUser->password, password) != 0)
    {
        printf("                                    Invalid password. Login failed.\n");
        return NULL;
    }

    printf("                                    Login successful. Welcome, %s!\n", username);
    return currentUser;
}


// view user profile
void viewUserProfile(User* user)
{
    if (user == NULL)
    {
        printf("                                    No user is currently logged in.\n");
        return;
    }

    printf("\n                                    User Profile:\n");
    printf("                                    ----------------------\n");
    printf("                                    Full Name: %s\n", user->fullName);
    printf("                                    Username: %s\n", user->username);
    printf("                                    User Type: %s\n", user->userType);
    printf("                                    ----------------------\n");
}


// update user profile
void updateUserProfile(User* user)
{
    if (user == NULL)
    {
        printf("                                    No user is currently logged in.\n");
        return;
    }

    printf("\n                                    Update User Profile:\n");
    printf("                                    1. Update Name\n");
    printf("                                    2. Update Username\n");
    printf("                                    3. Update Password\n");
    printf("                                    Enter your choice: ");

    int choice;
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
        printf("                                    Enter new Name: ");
        scanf(" %[^\n]s", &user->fullName);
        printf("                                    Name updated successfully.\n");
        break;

    case 2:
        printf("                                    Enter new Username: ");
        scanf("%s", &user->username);
        printf("                                    Username updated successfully.\n");
        break;

    case 3:
        printf("                                    Enter new Password: ");
        scanf("%s", &user->password);
        printf("                                    Password updated successfully.\n");
        break;

    default:
        printf("                                    Invalid choice. Please try again.\n");
        break;
    }
}

// view available products
void viewProducts(Product* productList)
{
    if (productList == NULL)
    {
        printf("                                    No products are available.\n");
        return;
    }

    printf("\n                                    Available Products:\n");
    printf("                                    ---------------------------------------------------\n");
    printf("                                    Product Name\tPrice\tQuantity\n");
    printf("                                    ---------------------------------------------------\n");

    Product* current = productList;
    while (current != NULL)
    {
        printf("                                    %s\t%.2lf\t%d\n", current->name, current->price, current->quantity);
        current = current->next;
    }

    printf("                                    ---------------------------------------------------\n");
}

// search for a product
void searchProduct(Product* productList)
{
    if (productList == NULL)
    {
        printf("                                    No products are available to search.\n");
        return;
    }

    char searchQuery[50];
    printf("\n                                    Enter the product name or search query: ");
    scanf("%s", searchQuery);

    printf("\n                                    Search Results:\n");
    printf("                                    ---------------------------------------------------\n");
    printf("                                    Product Name\tPrice\tQuantity\n");
    printf("                                    ---------------------------------------------------\n");

    Product* current = productList;
    while (current != NULL)
    {
        // Check if the product name contains the search query
        if (strstr(current->name, searchQuery) != NULL)
        {
            printf("                                    %s\t%.2lf\t%d\n", current->name, current->price, current->quantity);
        }
        current = current->next;
    }

    printf("                                    ---------------------------------------------------\n");
}

// add a product to the user's shopping cart
void addToCart(User* user, Product* productList, CartItem** shoppingCart)
{
    /*if (user == NULL)
    {
        printf("No user is currently logged in. Please sign in or log in.\n");
        return;
    }*/
    if (productList == NULL)
    {
        printf("\n                                    Product list is empty.\n");
        return;
    }

    Product* current = productList;
    printf("\n                                    Product Name\tPrice\tQuantity\n");
    while (current != NULL)
    {
        printf("                                    %s\t%.2lf\t%d\n", current->name, current->price, current->quantity);

        current = current->next;
    }

    char productName[50];
    printf("\n                                    Enter the product name to add to your cart: ");
    scanf("%s", productName);

    Product* productToAdd = findProduct(productList, productName);
    if (productToAdd == NULL)
    {
        printf("                                    Product '%s' not found. Please enter a valid product name.\n", productName);
        return;
    }

    int quantityToAdd;
    printf("                                    Enter the quantity to add to your cart: ");
    scanf("%d", &quantityToAdd);

    if (quantityToAdd <= 0 || quantityToAdd > productToAdd->quantity)
    {
        printf("                                    Invalid quantity. Please enter a valid quantity.\n");
        return;
    }

    // Create a new shopping cart item
    CartItem* newItem = (CartItem*)malloc(sizeof(CartItem));
    if (newItem == NULL)
    {
        printf("                                    Cannot add the product to your cart.\n");
        return;
    }

    strcpy(newItem->productName, productToAdd->name);
    newItem->price = productToAdd->price;
    newItem->quantity = quantityToAdd;
    newItem->next = NULL;

    // Add the new item to the user's shopping cart
    if (*shoppingCart == NULL)
    {
        *shoppingCart = newItem;
    }
    else
    {
        // Add the new item to the end of the shopping cart
        CartItem* current = *shoppingCart;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = newItem;
    }

    printf("                                    Product '%s' added to your cart.\n", productToAdd->name);
}

// view and manage the user's shopping cart
void viewShoppingCart(User* user, CartItem** shoppingCart, Product* productList)
{
    if (user == NULL)
    {
        printf("\n                                    No user is currently logged in. Please sign in or log in.\n");
        return;
    }

    if (*shoppingCart == NULL)
    {
        printf("\n                                    Your shopping cart is empty.\n");
        return;
    }

    printf("\n                                    Shopping Cart Contents:\n");
    printf("                                    ---------------------------------------------------\n");
    printf("                                    Product Name\tPrice\tQuantity\n");
    printf("                                    ---------------------------------------------------\n");

    CartItem* current = *shoppingCart;
    while (current != NULL)
    {
        printf("                                    %s\t%.2lf\t%d\n", current->productName, current->price, current->quantity);
        current = current->next;
    }

    printf("                                    ---------------------------------------------------\n");

    int cartChoice;
    while (1)
    {
        printf("\n                                    Cart Management Options:\n");
        printf("                                    1. Remove Item\n");
        printf("                                    2. Proceed to Checkout\n");
        printf("                                    3. Back to User Panel\n");
        printf("                                    Enter your choice: ");
        scanf("%d", &cartChoice);

        switch (cartChoice)
        {
        case 1: // Remove Item
            removeFromCart(shoppingCart);
            break;

        case 2: // Proceed to Checkout
            checkout(user, shoppingCart, productList);
            return;

        case 3: // Back to User Panel
            return;

        default:
            printf("                                    Invalid choice. Please try again.\n");
        }
    }
}

// remove an item from the shopping cart
void removeFromCart(CartItem** shoppingCart)
{
    char productName[50];
    printf("\n                                    Enter the product name to remove from your cart: ");
    scanf("%s", productName);

    CartItem* current = *shoppingCart;
    CartItem* previous = NULL;

    while (current != NULL)
    {
        if (strcmp(current->productName, productName) == 0)
        {
            if (previous == NULL)
            {
                *shoppingCart = current->next;
            }
            else
            {
                previous->next = current->next;
            }
            free(current);
            printf("                                    Product '%s' removed from your cart.\n", productName);
            return;
        }

        previous = current;
        current = current->next;
    }

    printf("                                    Product '%s' not found in your cart.\n", productName);
}

// Process checkout
void checkout(User* user, CartItem** shoppingCart, Product* productList)
{
    if (user == NULL)
    {
        printf("\n                                    No user is currently logged in. Please sign in or log in.\n");
        return;
    }

    if (*shoppingCart == NULL)
    {
        printf("\n                                    Your shopping cart is empty. Add items to your cart first.\n");
        return;
    }

    double totalCost = 0;

    CartItem* currentCartItem = *shoppingCart;
    Product* currentProduct;

    // Create a file to store sold products
    FILE* soldProductsFile = fopen("sold_products.txt", "a");
    if (soldProductsFile == NULL)
    {
        printf("Failed to open the file for sold products.\n");
        return;
    }

    while (currentCartItem != NULL)
    {
        currentProduct = findProduct(productList, currentCartItem->productName);

        if (currentProduct == NULL)
        {
            printf("Product '%s' not found in the product list. Please review your cart.\n", currentCartItem->productName);
            fclose(soldProductsFile);
            return;
        }

        if (currentProduct->quantity < currentCartItem->quantity)
        {
            printf("Not enough stock available for product '%s'. Please update your cart.\n", currentCartItem->productName);
            fclose(soldProductsFile);
            return;
        }

        // Update the soldQuantity and deduct the quantity from available stock
        currentProduct->soldQuantity += currentCartItem->quantity;
        currentProduct->quantity -= currentCartItem->quantity;

        totalCost += currentCartItem->quantity * currentProduct->price;

        // Store sold product info
        fprintf(soldProductsFile, "Product Name: %s, Price: %.2lf, Quantity Sold: %d\n", currentProduct->name, currentProduct->price, currentCartItem->quantity);

        currentCartItem = currentCartItem->next;
    }

    int paymentChoice;
    char paymentMethod[20];
    long cardNumber = 0;
    int number = 0;

    printf("\n                                    Choose a payment method:\n");
    printf("                                    1. Credit Card\n");
    printf("                                    2. Bkash/Nagad/Rocket\n");
    printf("                                    3. Cash on Delivery\n");
    scanf("%d", &paymentChoice);

    switch (paymentChoice)
    {
        case 1: // Credit Card
            printf("                                    Enter your credit card number: ");
            scanf("%ld", &cardNumber);
            strcpy(paymentMethod, "Credit Card");
            printf("                                    Enter your password: ");
            scanf("%ld", &number);
            break;

        case 2: // Bkash/Nagad/Rocket
        {
            int paymentOption;
            printf("                                    Choose one from Bkash (1), Nagad (2), or Rocket (3): ");
            scanf("%d", &paymentOption);

            switch (paymentOption)
            {
                case 1:
                    strcpy(paymentMethod, "Bkash");
                    printf("                                    Enter your Bkash number: ");
                    scanf("%ld", &number);
                    break;
                case 2:
                    strcpy(paymentMethod, "Nagad");
                    printf("                                    Enter your Nagad number: ");
                    scanf("%ld", &number);
                    break;
                case 3:
                    strcpy(paymentMethod, "Rocket");
                    printf("                                    Enter your Rocket number: ");
                    scanf("%ld", &number);
                    break;
                default:
                    printf("                                    Invalid choice. Defaulting to Bkash.\n");
                    strcpy(paymentMethod, "Bkash");
                    break;
            }
        }
        break;

        case 3: // Cash on Delivery
            strcpy(paymentMethod, "Cash on Delivery");
            break;

        default:
            printf("                                    Invalid payment method.\n");
            fclose(soldProductsFile);
            return;
    }

    fclose(soldProductsFile);

    printf("                                    Checkout successful. Total cost: $%.2lf\n", totalCost);

    // Call the showInvoice function
    showInvoice(*shoppingCart, user, totalCost, paymentMethod);

    // Prompt users to leave reviews for each product in the cart
    currentCartItem = *shoppingCart;
    while (currentCartItem != NULL)
    {
        leaveReview(currentCartItem->product, user);
        currentCartItem = currentCartItem->next;
    }

    // Clear the shopping cart after successful checkout
    freeCart(*shoppingCart);
    *shoppingCart = NULL;
}

// show invoice
void showInvoice(CartItem* cart, User* user, double totalCost, const char* paymentMethod)
{
    FILE* invoiceFile = fopen("invoice.txt", "a");
    if (invoiceFile == NULL)
    {
        printf("Unable to open or create invoice.txt.\n");
        return;
    }

    fprintf(invoiceFile, "\nInvoice for User: %s\n", user->username);
    fprintf(invoiceFile, "-------------------------------------\n");

    double totalAmount = totalCost;
    CartItem* currentCartItem = cart;
    while (currentCartItem != NULL)
    {
        fprintf(invoiceFile, "Product: %s, Price: %.2lf, Quantity: %d\n",
                currentCartItem->productName, currentCartItem->price, currentCartItem->quantity);
        currentCartItem = currentCartItem->next;
    }

    fprintf(invoiceFile, "-------------------------------------\n");
    fprintf(invoiceFile, "Payment Method: %s\n", paymentMethod);
    fprintf(invoiceFile, "Total Amount: $%.2lf\n", totalAmount);
    fprintf(invoiceFile, "-------------------------------------\n");
    fclose(invoiceFile);

    printf("                                    Thanks for purchasing our product! We will send the Invoice to you by mail.\n");
}

// leave review
void leaveReview(Product* product, User* user)
{
    char review[2000];
    int rating;

    printf("                                    Leave a review for the product '%s': ", product->name);
    getchar();
    fgets(review, sizeof(review), stdin);

    printf("                                    Rate the product from 0 (worst) to 5 (best): ");
    scanf("%d", &rating);
    if (rating < 0)
    {
        rating = 0;
    }
    else if (rating > 5)
    {
        rating = 5;
    }

    FILE* reviewFile = fopen("review.txt", "a");
    if (reviewFile == NULL)
    {
        printf("                                    Something went wrong. Sorry!\n");
        return;
    }

    fprintf(reviewFile, "\nReview for Product: %s\n", product->name);
    fprintf(reviewFile, "User: %s\n", user->username);
    fprintf(reviewFile, "Rating: %d/5\n", rating);
    fprintf(reviewFile, "Review: %s", review);
    fprintf(reviewFile, "---------------------------\n");
    fclose(reviewFile);

    printf("                                    Thanks so much for your valuable opinion!\n");
}

int main()
{
    system("color 9f");

    int choice;
    User* userList = loadUserData();
    Product* productList = loadProductData();
    CartItem* shoppingCart = NULL;

    while (1)
    {
        displayMainMenu();
        printf("                                    Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1: // Admin Panel
            if (verifyAdminPassword())
            {
                adminPanel(&userList, &productList);
            }
            else
            {
                printf("                                    Incorrect admin password. Access denied.\n");
            }
            break;

        case 2: // User Panel
            userPanel(&userList, &productList, &shoppingCart);
            break;

        case 3: // Exit
            saveUserToFile(userList);
            saveProductToFile(productList);
            freeCart(shoppingCart);
            printf("                                    Exiting the program. Goodbye!\n");
            exit(0);

        default:
            printf("                                    Invalid choice. Please try again.\n");
            break;
        }
    }

    return 0;
}
